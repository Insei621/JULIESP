#include "../include/SemanticAnalyzer.h"

void SemanticAnalyzer::analyze(ASTNode* root) {
    if (!root) return;

    // Réinitialisation des statistiques
    count_sexpr = count_symbols = count_primitives = count_literals = 0;

    try {
        root->accept(this);
        //std::cout << "\033[1;32m[Succès]\033[0m Sémantique validée avec succès." << std::endl;
    } catch (const std::exception& e) {
        throw; // Remonte l'exception au main pour qu'il gère l'affichage
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
            Identifier* id = dynamic_cast<Identifier*>(children[1]);
            if (!id) throw std::runtime_error("Le premier argument de ':' doit être un identifiant.");

            // Puis on enregistre le symbole
            symtable.enter(id->getName(), children[2]);

            if (children.size() < 3)
                throw std::runtime_error("L'opérateur ':' attend un nom et une valeur.");

            // On analyse la valeur uniquement
            children[2]->accept(this);
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
        // << >> | : attendent une liste — PAS @
        if (op == "cdr" || op == "car" || op == "null") {
            if (children.size() >= 2) {
                if (dynamic_cast<IntegerLit*>(children[1]) ||
                    dynamic_cast<FloatLit*>(children[1])) {
                    throw std::runtime_error(
                        "'" + op + "' attend une liste mais reçoit un littéral numérique. "
                        "[Ligne " + std::to_string(children[1]->getLine()) + "]"
                    );
                    }
            }
        }

        // & : le 2ème arg doit être une liste ou ²()
        if (op == "cons") {
            if (children.size() >= 3) {
                if (dynamic_cast<IntegerLit*>(children[2]) ||
                    dynamic_cast<FloatLit*>(children[2])) {
                    throw std::runtime_error(
                        "'&' attend une liste comme 2ème argument. "
                        "Utilise ²() pour la liste vide. "
                        "[Ligne " + std::to_string(children[2]->getLine()) + "]"
                    );
                    }
            }
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

// --- VISITE DES IDENTIFIANTS (Vérification des variables) ---
void SemanticAnalyzer::visit(Identifier* node) {

    if (node->isQuotedNode()) {
        return; // Le ² nous dis de ne pas evaluer
    }

    count_symbols++;
    std::string name = node->getName();

    // Exclusion des constantes littérales
    if (name == "ù" || name == "µ" || name == "nil") return;

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