#include "../include/SemanticAnalyzer.h"

void SemanticAnalyzer::analyze(ASTNode* root) {
    if (!root) return;

    // Réinitialisation des statistiques
    count_sexpr = count_symbols = count_primitives = count_literals = 0;

    std::cout << "\033[1;34m[Sémantique]\033[0m Lancement de l'analyse...";

    try {
        root->accept(this);
        Scope* global = symtable.getGlobalScope();
        std::cout << "\n--- RÉSULTAT DES TABLES DES SYMBOLES ---" << std::endl;
        // On récupère le scope global (la racine de l'arbre) pour tout afficher
        printAllScopes(symtable.getCurrentScope(), 0);        std::cout << "----------------------------------------" << std::endl;
        std::cout << "\033[1;32m[Succès]\033[0m Sémantique validée avec succès." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "\n\033[1;31m[ERREUR SÉMANTIQUE]\033[0m " << e.what() << std::endl;
        // On ne coupe pas le programme ici pour laisser le main gérer l'exit
    }
}

// --- VISITE DES S-EXPRESSIONS (Logique des Scopes) ---
void SemanticAnalyzer::visit(SExpr* node) {
    // 1. LE BOUCLIER QUOTE : Si c'est cité, on n'analyse rien à l'intérieur
    if (node->isQuotedNode()) return;

    count_sexpr++;
    auto children = node->getChildren();
    if (children.empty()) return;

    // On regarde si le premier élément est une primitive ( : , £ , ; , + , etc.)
    Primitive* prim = dynamic_cast<Primitive*>(children[0]);
    if (prim) {
        std::string op = prim->getName();

        // --- CAS : AFFECTATION ( : nom valeur ) ---
        if (op == "setq") {
            if (children.size() < 3)
                throw std::runtime_error("L'opérateur ':' attend un nom et une valeur.");

            Identifier* id = dynamic_cast<Identifier*>(children[1]);
            if (!id) throw std::runtime_error("Le premier argument de ':' doit être un identifiant.");

            // ✅ Ne pas visiter le nom ici !

            // On analyse la valeur uniquement
            children[2]->accept(this);

            // Puis on enregistre le symbole
            symtable.enter(id->getName(), children[2]);

            return;
        }


        // --- CAS : FONCTION ANONYME ( £ (params) corps ) ---
        if (op == "lambda") {
            symtable.openScope(); // On crée un nouvel espace pour les variables locales

            if (children.size() > 1) {
                // On récupère la liste des paramètres ( l x y ... )
                if (auto paramList = dynamic_cast<SExpr*>(children[1])) {
                    for (auto paramNode : paramList->getChildren()) {
                        if (auto paramId = dynamic_cast<Identifier*>(paramNode)) {
                            // On déclare chaque paramètre dans le scope local
                            symtable.enter(paramId->getName(), paramId);
                        }
                    }
                }
            }

            // On analyse le corps de la fonction (maintenant il connaît ses paramètres !)
            if (children.size() > 2) children[2]->accept(this);

            symtable.closeScope(); // On détruit le scope local en sortant
            return;
        }

        // --- CAS : BLOC DE CODE ( ; expr1 expr2 ... ) ---
        if (op == "progn") {
            for (size_t i = 1; i < children.size(); ++i) {
                children[i]->accept(this);
            }
            return;
        }
    }

    // --- VISITE STANDARD ---
    // Si ce n'est pas une forme spéciale (ex: (+ 1 2) ou (ma_fonction x)),
    // on analyse tous les enfants normalement.
    for (auto child : children) {
        if (auto prim = dynamic_cast<Primitive*>(children[0])) {
            if (prim->getName() == "setq" ||
                prim->getName() == "lambda" ||
                prim->getName() == "progn") {
                // déjà traité
                return;
                }
        }
        child->accept(this);
    }
}
/*
void SemanticAnalyzer::visit(SExpr* node) {
    // --- LE BOUCLIER QUOTE ---
    if (node->isQuotedNode()) {
        return; // On stoppe l'analyse ici pour ce nœud et ses enfants
    }

    count_sexpr++;
    auto children = node->getChildren();
    if (children.empty()) return;

    // Analyse normale (non-quoted)
    Primitive* prim = dynamic_cast<Primitive*>(children[0]);
    if (prim) {
        std::string op = prim->getName();

        // --- CAS : AFFECTATION ( : nom valeur ) ---
        if (op == ":") {
            if (children.size() < 3) throw std::runtime_error("':' attend un nom et une valeur.");

            // 1. On récupère le nom SANS le visiter (pour éviter le lookup prématuré)
            Identifier* id = dynamic_cast<Identifier*>(children[1]);
            if (!id) throw std::runtime_error("Le premier argument de ':' doit être un identifiant.");

            // 2. On analyse la valeur (le 3ème enfant)
            children[2]->accept(this);

            // 3. On enregistre le symbole dans le scope actuel
            symtable.enter(id->getName(), children[2]);
            return;
        }

        // --- CAS : FONCTION ANONYME ( £ (params) corps ) ---
        if (op == "£") {
            symtable.openScope(); // On entre dans le monde de la fonction

            // On enregistre les paramètres (le 2ème enfant)
            if (children.size() > 1) {
                // Logique pour extraire les noms des paramètres et les "enter" dans le scope
            }

            // On visite le corps (le 3ème enfant)
            if (children.size() > 2) children[2]->accept(this);

            symtable.closeScope(); // On ressort
            return;
        }

        // --- CAS : BLOC DE CODE ( ; expr1 expr2 ... ) ---
        if (op == ";") {
            for (size_t i = 1; i < children.size(); ++i) {
                children[i]->accept(this);
            }
            return;
        }

        // Si c'est une primitive normale ( + , - , etc.), on laisse filer vers la visite standard
    }

    // Visite standard pour les appels de fonctions
    for (auto child : children) {
        child->accept(this);
    }
}
*/
// --- VISITE DES IDENTIFIANTS (Vérification des variables) ---
void SemanticAnalyzer::visit(Identifier* node) {

    if (node->isQuotedNode()) {
        return; // Le ² nous dis de ne pas evaluer
    }

    count_symbols++;
    std::string name = node->getName();

    // Exclusion des constantes littérales
    if (name == "ù" || name == "µ") return;

    if (symtable.lookup(name) == nullptr) {
        // On récupère la ligne et la colonne depuis le nœud
        std::string loc = " [Ligne " + std::to_string(node->getLine()) +
                          ", Col " + std::to_string(node->getColumn()) + "]";

        throw std::runtime_error("Le symbole '" + name + "' n'est pas défini dans ce scope." + loc);
    }
}

// --- VISITE DES AUTRES NŒUDS ---
void SemanticAnalyzer::visit(Primitive* node) {
    count_primitives++;
}

void SemanticAnalyzer::visit(IntegerLit* node) { count_literals++; }
void SemanticAnalyzer::visit(FloatLit* node)   { count_literals++; }
void SemanticAnalyzer::visit(StringLit* node)  { count_literals++; }
void SemanticAnalyzer::visit(CharLit* node)    { count_literals++; }
void SemanticAnalyzer::visit(BoolLit* node)    { count_literals++; }


void SemanticAnalyzer::printAllScopes(Scope* s, int indent) {
    if (!s) {
        std::cout << "DEBUG: Scope nul reçu à l'indentation " << indent << std::endl;
        return;
    }

    std::string prefix(indent * 4, ' ');
    // On récupère une référence CONSTANTE pour éviter les copies vides
    const auto& symbols = s->getSymbols();
    const auto& children = s->getChildren();

    std::cout << prefix << (indent == 0 ? "[RACINE] GLOBAL SCOPE" : "[ENFANT] LOCAL SCOPE")
              << " (" << symbols.size() << " symboles, " << children.size() << " enfants)" << std::endl;

    for (auto const& [name, node] : symbols) {
        // On affiche le nom entre guillemets pour détecter des espaces cachés
        std::cout << prefix << "  ├── \"" << name << "\"" << std::endl;
    }

    for (Scope* child : children) {
        printAllScopes(child, indent + 1);
    }
}