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
    count_sexpr++;
    auto children = node->getChildren();
    if (children.empty()) return;

    Primitive* prim = dynamic_cast<Primitive*>(children[0]);

    if (prim) {
        std::string op = prim->getName();

        // --- 1. SETQ / DÉCLARATION (:) ---
        if (op == "setq") {
            if (children.size() >= 3) {
                // ASTUCE : On enregistre le symbole TOUT DE SUITE avec une valeur nulle (ou le nœud lui-même)
                // pour que les appels récursifs à l'intérieur de children[2] le trouvent.
                symtable.enter(children[1]->getName(), children[2]);

                // Maintenant on visite la valeur (le lambda, etc.)
                children[2]->accept(this);
                return;
            }
        }

        // --- 2. LAMBDA / FONCTION (£) ---
        else if (op == "lambda") {
            symtable.openScope(); // On crée la table locale

            // Gestion des arguments : on les enregistre sans les "visiter"
            if (children.size() >= 2) {
                SExpr* argList = dynamic_cast<SExpr*>(children[1]);
                if (argList) {
                    for (ASTNode* arg : argList->getChildren()) {
                        // On enregistre le nom de l'argument pour le lookup futur
                        symtable.enter(arg->getName(), arg);
                    }
                }
            }

            // On visite le corps (souvent un progn)
            for (size_t i = 2; i < children.size(); ++i) {
                children[i]->accept(this);
            }

            symtable.closeScope();
            return;
        }

        // --- 3. PROGN / BLOC (;) ---
        else if (op == "progn") {
            // IMPORTANT : Progn ne doit pas ouvrir de scope sinon
            // les 'setq' à l'intérieur disparaissent à la sortie du bloc.
            for (size_t i = 1; i < children.size(); ++i) {
                children[i]->accept(this);
            }
            return;
        }
    }

    // Visite standard pour le reste (Appels de fonctions, IF, etc.)
    for (auto child : children) {
        child->accept(this);
    }
}
// --- VISITE DES IDENTIFIANTS (Vérification des variables) ---
void SemanticAnalyzer::visit(Identifier* node) {
    count_symbols++;
    std::string name = node->getName();

    // Exclusion des symboles "booléens" qui représente la fin des listes
    if (name == "ù") return;

    // On cherche dans la pile de scopes (remonte jusqu'au global)
    if (symtable.lookup(name) == nullptr) {
        throw std::runtime_error("Le symbole '" + name + "' n'est pas défini dans ce scope.");
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