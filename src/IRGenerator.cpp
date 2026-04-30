//
// IRGenerator.cpp — Implémentation du visiteur de génération d'IR
//

#include "../include/pch.h"
#include "../include/IRGenerator.h"


// =============================================================================
// Constructeur & point d'entrée
// =============================================================================

IRGenerator::IRGenerator()
    : currentBlock_(&program_.mainBlock)
    , tempCounter_(0)
    , labelCounter_(0)
    , funcCounter_(0)
{}

IRProgram IRGenerator::generate(ASTNode* root) {
    program_      = IRProgram{};
    currentBlock_ = &program_.mainBlock;
    tempCounter_  = 0;
    labelCounter_ = 0;
    funcCounter_  = 0;
    lastResult_   = "";
    typeTable_.clear();

    // La racine est une SExpr "conteneur" — on visite chaque enfant directement
    if (auto* rootExpr = dynamic_cast<SExpr*>(root)) {
        for (ASTNode* child : rootExpr->getChildren()) {
            child->accept(this);
        }
    } else {
        root->accept(this); // fallback si ce n'est pas une SExpr
    }

    return program_;
}

// =============================================================================
// Utilitaires privés
// =============================================================================

IROperand IRGenerator::newTemp(IRType type) {
    // On génère un nom "t0", "t1", etc.
    std::string name = "t" + std::to_string(tempCounter_++);
    // On émet une instruction d'assignation avec une valeur vide — ce sera
    // rempli juste après. En pratique on émet la vraie instruction directement,
    // donc newTemp() sert juste à réserver le nom.
    // (On n'émet PAS de déclaration ici — c'est l'instruction qui porte le type.)
    return name;
}

std::string IRGenerator::newLabel(const std::string& prefix) {
    return prefix + "_" + std::to_string(labelCounter_++);
}

std::string IRGenerator::newFuncName(const std::string& hint) {
    if (!hint.empty()) {
        // On sanitise le nom (remplace - par _ pour être valide en C)
        std::string safe = hint;
        std::replace(safe.begin(), safe.end(), '-', '_');
        return safe;
    }
    return "__fn" + std::to_string(funcCounter_++);
}

void IRGenerator::emit(IRInstruction instr) {
    currentBlock_->emit(std::move(instr));
}

// =============================================================================
// Inférence de type basique
// =============================================================================
//
// Pour l'instant, on fait une inférence locale simple.
// On n'a pas de vrai système de types — on retourne UNKNOWN si on ne sait pas.
// Le générateur C pourra traiter UNKNOWN comme "int" par défaut.
//

IRType IRGenerator::inferType(ASTNode* node) {
    // Littéraux → type direct
    if (dynamic_cast<IntegerLit*>(node))  return IRType::INT;
    if (dynamic_cast<FloatLit*>(node))    return IRType::FLOAT;
    if (dynamic_cast<CharLit*>(node))     return IRType::CHAR;
    if (dynamic_cast<StringLit*>(node))   return IRType::STRING;
    if (dynamic_cast<BoolLit*>(node))     return IRType::BOOL;

    // Identifier → on cherche dans la table des types
    if (auto* id = dynamic_cast<Identifier*>(node)) {
        auto it = typeTable_.find(id->getName());
        if (it != typeTable_.end()) return it->second;
    }

    // SExpr → on regarde le premier enfant pour déduire le type de retour
    if (auto* sexpr = dynamic_cast<SExpr*>(node)) {
        if (!sexpr->getChildren().empty()) {
            ASTNode* head = sexpr->getChildren()[0];
            if (auto* prim = dynamic_cast<Primitive*>(head)) {
                std::string name = prim->getName();
                // Les opérateurs arithmétiques retournent le type de leur 1er opérande
                if (name == "+" || name == "-" || name == "*" || name == "/") {
                    if (sexpr->getChildren().size() > 1)
                        return inferType(sexpr->getChildren()[1]);
                }
                // Les comparaisons retournent toujours un bool (int en C)
                if (name == "<" || name == ">" || name == "=" ||
                    name == "<=" || name == ">=")
                    return IRType::BOOL;
            }
        }
    }

    return IRType::UNKNOWN;
}

// =============================================================================
// VISIT : Littéraux
// =============================================================================
//
// Pour chaque littéral, on met simplement sa valeur textuelle dans lastResult_.
// On n'émet PAS d'instruction : une constante n'a pas besoin d'un temporaire
// tant qu'elle n'est pas utilisée dans une opération.
//

void IRGenerator::visit(IntegerLit* node) {
    lastResult_ = std::to_string(node->value);
}

void IRGenerator::visit(FloatLit* node) {
    // On force le "f" pour que le générateur C produise un float et pas un double
    lastResult_ = std::to_string(node->value) + "f";
}

void IRGenerator::visit(CharLit* node) {
    // On enveloppe dans des apostrophes pour le C : 'a'
    lastResult_ = std::string("'") + node->value + "'";
}

void IRGenerator::visit(StringLit* node) {
    // On enveloppe dans des guillemets pour le C : "hello"
    lastResult_ = "\"" + node->value + "\"";
}

void IRGenerator::visit(BoolLit* node) {
    // En C, true = 1, false = 0
    lastResult_ = node->value ? "1" : "0";
}

// =============================================================================
// VISIT : Identifier
// =============================================================================
//
// Un identifiant est une référence à une variable existante.
// On met juste son nom dans lastResult_ (après sanitisation pour le C).
//

void IRGenerator::visit(Identifier* node) {
    std::string name = node->getName();
    if (node->isQuotedNode()) {
        lastResult_ = "\"" + name + "\""; // Transforme le symbole ²hello en "hello"
        return;
    }

    // 2. Nettoyage du nom pour le C (ex: rayon-cercle -> rayon_cercle)
    std::replace(name.begin(), name.end(), '-', '_');
    lastResult_ = name;
}
// =============================================================================
// VISIT : Primitive
// =============================================================================
//
// Une primitive (car, cdr, cons, etc.) au niveau ATOME (pas dans une SExpr)
// est juste un nom de fonction. On met son nom dans lastResult_.
// Le cas d'utilisation réel (comme opérateur d'une SExpr) est géré dans visit(SExpr*).
//

void IRGenerator::visit(Primitive* node) {
    lastResult_ = node->getName();
}

// =============================================================================
// VISIT : SExpr — le cœur du générateur
// =============================================================================
//
// C'est ici que tout se passe. Une SExpr a la forme :
//   (operateur arg1 arg2 ...)
//
// L'opérateur (premier enfant) détermine quelle forme spéciale ou quel appel
// de fonction on doit générer.
//
// ATTENTION : une SExpr quotée (ex: '(1 2 3)) n'est PAS évaluée —
// elle représente une donnée. Pour l'instant on la traite comme un commentaire
// et on retourne une chaîne placeholder. (Les listes Lisp nécessiteraient
// une vraie représentation de listes au runtime, ce qu'on fera côté C.)
//

void IRGenerator::visit(SExpr* node) {

    // --- Cas 1 : SExpr vide (²() → NULL en C) ---
    if (node->getChildren().empty()) {
        lastResult_ = "NULL";  // ← était ""
        return;
    }

    // --- Cas 2 : SExpr quotée non vide → liste littérale (non supporté pour l'instant) ---
    if (node->isQuotedNode()) {
        lastResult_ = "NULL";  // ← était "/*quoted-list*/"
        return;
    }

    // --- Cas 3 : On inspecte le premier enfant pour identifier la forme ---
    ASTNode* head = node->getChildren()[0];

    // Est-ce une primitive ?
    if (auto* prim = dynamic_cast<Primitive*>(head)) {
        std::string primName = prim->getName();

        // Formes spéciales reconnues
        if      (primName == "if" || primName == "?")     { lastResult_ = handleIf(node);         return; }
        else if (primName == "setq" || primName == ":")   { lastResult_ = handleSetq(node);        return; }
        else if (primName == "lambda" || primName == "£") { lastResult_ = handleLambda(node);      return; }
        else if (primName == "progn" || primName == ";")  { lastResult_ = handleProgn(node);       return; }
        else if (primName == "print" || primName == "€")  { lastResult_ = handlePrint(node);       return; }
        else if (primName == "scan" || primName == "ç")   { lastResult_ = handleScan(node);        return; }

        // Opérateurs arithmétiques et de comparaison
        else if (primName == "+")  { lastResult_ = handleArithmetic(node, "+");  return; }
        else if (primName == "-")  { lastResult_ = handleArithmetic(node, "-");  return; }
        else if (primName == "*")  { lastResult_ = handleArithmetic(node, "*");  return; }
        else if (primName == "/")  { lastResult_ = handleArithmetic(node, "/");  return; }
        else if (primName == "<")  { lastResult_ = handleArithmetic(node, "<");  return; }
        else if (primName == ">")  { lastResult_ = handleArithmetic(node, ">");  return; }
        else if (primName == "=")  { lastResult_ = handleArithmetic(node, "=="); return; }
        else if (primName == "¤") { lastResult_ = handleArithmetic(node, "¤"); return; }

        // Primitives Lisp (car, cdr, cons...) → appels de fonctions runtime
        else { lastResult_ = handlePrimitive(node); return; }
    }

    if (auto* ident = dynamic_cast<Identifier*>(head)) {
        // Si l'identifiant est en fait un mot-clé (selon son type de token)
        // Note : il faut que ton AST conserve le type de token ou que tu
        // vérifies le nom si ton AST ne garde que le string.

        std::string name = ident->getName();
        if (name == "ç") { // Ou vérifie le TokenType si ton AST le permet
            lastResult_ = handleScan(node);
            return;
        }
    }
    // Est-ce un identifiant ? → appel de fonction utilisateur
    if (dynamic_cast<Identifier*>(head)) {
        lastResult_ = handleCall(node);
        return;
    }

    // Cas inattendu : on met un placeholder
    lastResult_ = "/*unknown-sexpr*/";
}

// =============================================================================
// Handlers des formes spéciales
// =============================================================================

// -----------------------------------------------------------------------------
// handleIf : (if cond then else?)
// -----------------------------------------------------------------------------
//
// On génère le pattern classique avec labels et sauts :
//
//   <évaluation de cond>   → résultat dans t_cond
//   if (t_cond) goto L_then_N; else goto L_else_N;
//   L_then_N:
//   <évaluation de then>   → résultat dans t_then
//   t_result = t_then
//   goto L_end_N;
//   L_else_N:
//   <évaluation de else>   → résultat dans t_else
//   t_result = t_else
//   L_end_N:
//

IROperand IRGenerator::handleIf(SExpr* node) {
    const auto& children = node->getChildren();

    children[1]->accept(this);
    IROperand condResult = lastResult_;

    std::string labelThen = newLabel("L_then");
    std::string labelElse = newLabel("L_else");
    std::string labelEnd  = newLabel("L_end");

    emit(IR_CondJump{ condResult, labelThen, labelElse });

    // Branche THEN
    emit(IR_Label{ labelThen });
    children[2]->accept(this);
    IROperand thenResult = lastResult_;
    emit(IR_Jump{ labelEnd });

    // Branche ELSE
    emit(IR_Label{ labelElse });
    IROperand elseResult = "";
    if (children.size() >= 4) {
        bool isNil = false;
        if (auto* id = dynamic_cast<Identifier*>(children[3]))
            isNil = (id->getName() == "nil");
        if (!isNil) {
            children[3]->accept(this);
            elseResult = lastResult_;
        }
    }

    emit(IR_Label{ labelEnd });

    // Ne crée un temporaire que si les branches retournent une vraie valeur
    if (!thenResult.empty() && !elseResult.empty()) {
        IROperand result = newTemp(IRType::UNKNOWN);
        // Insère les assignations AVANT les labels (dans le bon ordre)
        // On les émet juste avant le jump et avant le label de fin
        // → En pratique : retourne thenResult (le CGenerator choisira)
        return thenResult; // la valeur du then (utilisée si le if est une expr)
    }

    return ""; // void si les branches ne retournent rien
}


IROperand IRGenerator::handleSetq(SExpr* node) {
    const auto& children = node->getChildren();

    // 1. On récupère le nom de la variable (Cible)
    std::string varName = children[1]->getName();
    std::replace(varName.begin(), varName.end(), '-', '_');

    // 2. On identifie le nœud de valeur (Source)
    ASTNode* valueNode = children[2];

    // 3. Gestion spécifique des Lambdas (Fonctions nommées)
    if (auto* sexpr = dynamic_cast<SExpr*>(valueNode)) {
        if (!sexpr->getChildren().empty()) {
            if (auto* prim = dynamic_cast<Primitive*>(sexpr->getChildren()[0])) {
                if (prim->getName() == "lambda" || prim->getName() == "£") {
                    handleLambdaWithName(sexpr, varName);
                    // On part du principe qu'une fonction est un type spécial ou UNKNOWN en C
                    typeTable_[varName] = IRType::UNKNOWN;
                    return varName;
                }
            }
        }
    }

    // 4. Cas normal : Calcul de la valeur
    // Cas normal
    valueNode->accept(this);
    IROperand valueResult = lastResult_;

    // Cherche le type dans typeTable_ en priorité (couvre LIST, etc.)
    IRType type = IRType::UNKNOWN;
    auto it = typeTable_.find(valueResult);
    if (it != typeTable_.end() && it->second != IRType::UNKNOWN) {
        type = it->second;
    } else {
        type = inferType(valueNode);
    }

    typeTable_[varName] = type;
    emit(IR_Assign{ type, varName, valueResult });
    return varName;
}

// -----------------------------------------------------------------------------
// handleLambda : (lambda (params...) body...)
// -----------------------------------------------------------------------------
//
// C'est la forme la plus complexe.
// On doit :
//   1. Créer un nouveau IR_Block pour le corps de la fonction
//   2. Sauvegarder le bloc courant, pointer sur le nouveau bloc
//   3. Générer le corps dans ce nouveau bloc
//   4. Restaurer le bloc courant
//   5. Ajouter la fonction au programme IR
//   6. Retourner le nom de la fonction (pour pouvoir l'appeler)
//

IROperand IRGenerator::handleLambda(SExpr* node) {
    return handleLambdaWithName(node, "");  // nom générique si pas de setq
}

// Dans IRGenerator.cpp — même logique que handleLambda mais avec nom imposé
IROperand IRGenerator::handleLambdaWithName(SExpr* node, const std::string& name) {
    const auto& children = node->getChildren();
    std::string funcName = newFuncName(name);

    // --- Collecte les paramètres ---
    std::vector<std::pair<IRType, std::string>> params;
    if (auto* paramList = dynamic_cast<SExpr*>(children[1])) {
        for (ASTNode* p : paramList->getChildren()) {
            std::string pname = p->getName();
            std::replace(pname.begin(), pname.end(), '-', '_');
            params.push_back({ IRType::UNKNOWN, pname });
        }
    }

    // --- Crée la déclaration ---
    IR_FuncDecl decl;
    decl.returnType = IRType::UNKNOWN;
    decl.name       = funcName;
    decl.params     = params;

    // --- Crée le bloc du corps ---
    IR_Block bodyBlock;
    bodyBlock.name = funcName;

    // --- Enregistre les paramètres dans typeTable_ avant de générer le corps ---
    for (const auto& [ptype, pname] : params) {
        typeTable_[pname] = ptype; // UNKNOWN au départ, patché plus tard par handleCall
    }

    // --- Sauvegarde le bloc courant et bascule sur le corps ---
    IR_Block* savedBlock = currentBlock_;
    currentBlock_ = &bodyBlock;

    // --- Génère le corps ---
    IROperand lastBodyResult = "";
    for (size_t i = 2; i < children.size(); ++i) {
        children[i]->accept(this);
        lastBodyResult = lastResult_;
    }

    // --- Émet le return seulement si la dernière expression retourne une valeur ---
    if (!lastBodyResult.empty()) {
        // Déduit le type de retour
        IRType retType = IRType::UNKNOWN;
        auto it = typeTable_.find(lastBodyResult);
        if (it != typeTable_.end() && it->second != IRType::UNKNOWN) {
            retType = it->second;
        } else {
            // Cherche dans les BinOp du corps
            for (const auto& instr : bodyBlock.instructions) {
                if (std::holds_alternative<IR_BinOp>(instr)) {
                    const auto& b = std::get<IR_BinOp>(instr);
                    if (b.dest == lastBodyResult && b.type != IRType::UNKNOWN) {
                        retType = b.type; break;
                    }
                }
                if (std::holds_alternative<IR_Call>(instr)) {
                    const auto& c = std::get<IR_Call>(instr);
                    if (c.dest == lastBodyResult && c.type != IRType::UNKNOWN) {
                        retType = c.type; break;
                    }
                }
            }
        }
        decl.returnType = retType;
        emit(IR_Return{ lastBodyResult });
    }
    // Si lastBodyResult est vide → fonction void, pas de return

    // --- Restaure le bloc courant ---
    currentBlock_ = savedBlock;

    // --- Enregistre la fonction dans le programme ---
    program_.functions.emplace_back(decl, std::move(bodyBlock));

    // Enregistre le type de retour dans typeTable_ pour handleCall
    typeTable_[funcName] = decl.returnType;

    lastResult_ = funcName;
    return funcName;
}

// -----------------------------------------------------------------------------
// handleProgn : (progn expr1 expr2 ...)
// -----------------------------------------------------------------------------
//
// Évalue les expressions dans l'ordre et retourne la dernière valeur.
// C'est le cas le plus simple — aucune instruction spéciale.
//

IROperand IRGenerator::handleProgn(SExpr* node) {
    const auto& children = node->getChildren();
    IROperand result = "";
    for (size_t i = 1; i < children.size(); ++i) {
        children[i]->accept(this);
        result = lastResult_;
    }
    return result;
}

// -----------------------------------------------------------------------------
// handlePrint : (print expr)
// -----------------------------------------------------------------------------

IROperand IRGenerator::handlePrint(SExpr* node) {
    const auto& children = node->getChildren();
    children[1]->accept(this);
    IROperand val = lastResult_;

    // Cherche d'abord dans typeTable_ (couvre variables, paramètres patchés)
    IRType type = IRType::UNKNOWN;
    if (auto* id = dynamic_cast<Identifier*>(children[1])) {
        auto it = typeTable_.find(id->getName());
        if (it != typeTable_.end()) type = it->second;
    }
    // Si toujours inconnu, infère depuis le nœud AST
    if (type == IRType::UNKNOWN) type = inferType(children[1]);

    emit(IR_Print{ type, val });
    return "";
}

// -----------------------------------------------------------------------------
// handleScan : (scan var)
// -----------------------------------------------------------------------------

IROperand IRGenerator::handleScan(SExpr* node) {
    const auto& children = node->getChildren();
    std::string varName = children[1]->getName();
    std::replace(varName.begin(), varName.end(), '-', '_');
    IRType type = inferType(children[1]);
    emit(IR_Scan{ type, varName });
    return varName;
}

// -----------------------------------------------------------------------------
// handleArithmetic : (+, -, *, /, <, >, ==, ...)
// -----------------------------------------------------------------------------
//
// On gère les cas N-aires : (+ 1 2 3 4)
// On les décompose en opérations binaires successives :
//   t0 = 1 + 2
//   t1 = t0 + 3
//   t2 = t1 + 4
//

IROperand IRGenerator::handleArithmetic(SExpr* node, const std::string& op) {
    const auto& children = node->getChildren();

    // Cas spécifique pour l'égalité d'adresse JulieSP -> C
    if (op == "¤") {
        children[1]->accept(this);
        IROperand left = lastResult_;

        children[2]->accept(this);
        IROperand right = lastResult_;

        IROperand dest = newTemp(IRType::BOOL);
        // On demande explicitement au générateur C d'écrire "=="
        emit(IR_BinOp{ IRType::BOOL, dest, left, "==", right });
        return dest;
    }

    // --- Reste de ta fonction pour +, -, *, / ---
    children[1]->accept(this);
    IROperand acc = lastResult_;
    IRType type = inferType(children[1]);

    for (size_t i = 2; i < children.size(); ++i) {
        children[i]->accept(this);
        IROperand right = lastResult_;
        IROperand dest = newTemp(type);
        emit(IR_BinOp{ type, dest, acc, op, right });
        acc = dest;
    }
    return acc;
}

// -----------------------------------------------------------------------------
// handlePrimitive : car, cdr, cons, null?, atom?, number?
// Compilés en appels de fonctions C runtime (on suppose qu'elles existent)
// -----------------------------------------------------------------------------

IROperand IRGenerator::handlePrimitive(SExpr* node) {
    const auto& children = node->getChildren();
    std::string primName = children[0]->getName();

    if (primName == "¤") return handleArithmetic(node, "==");

    // Collecte les arguments
    std::vector<IROperand> args;
    for (size_t i = 1; i < children.size(); ++i) {
        children[i]->accept(this);
        args.push_back(lastResult_);
    }

    std::string cName;
    IRType retType = IRType::UNKNOWN;

    if      (primName == "<<") { cName = "lisp_car";  retType = IRType::INT;  }
    else if (primName == ">>") { cName = "lisp_cdr";  retType = IRType::LIST; }
    else if (primName == "&")  { cName = "lisp_cons"; retType = IRType::LIST; }
    else if (primName == "|")  { cName = "lisp_null"; retType = IRType::BOOL; }
    else if (primName == "@")  { cName = "lisp_atom"; retType = IRType::BOOL; }
    else if (primName == "°")  { cName = "lisp_numberp"; retType = IRType::BOOL; }
    else {
        cName = "lisp_" + primName;
        for (char& ch : cName) {
            if (ch == '?' || ch == '-' || ch == '<' ||
                ch == '>' || ch == '&' || ch == '|' || ch == '@')
                ch = '_';
        }
    }

    IROperand dest = newTemp(retType);
    // Enregistre le type dans typeTable_ pour propagation
    typeTable_[dest] = retType;
    emit(IR_Call{ retType, dest, cName, args });
    return dest;
}


// -----------------------------------------------------------------------------
// handleCall : appel de fonction utilisateur (funcName arg1 arg2 ...)
// -----------------------------------------------------------------------------

IROperand IRGenerator::handleCall(SExpr* node) {
    const auto& children = node->getChildren();
    std::string funcName = children[0]->getName();
    std::replace(funcName.begin(), funcName.end(), '-', '_');

    // --- Évalue les arguments et collecte leurs types ---
    std::vector<IROperand> args;
    std::vector<IRType>    argTypes;
    for (size_t i = 1; i < children.size(); ++i) {
        children[i]->accept(this);
        args.push_back(lastResult_);

        // Cherche le type de l'argument : typeTable_ en priorité, sinon inferType
        IRType t = IRType::UNKNOWN;
        if (auto* id = dynamic_cast<Identifier*>(children[i])) {
            auto it = typeTable_.find(id->getName());
            if (it != typeTable_.end()) t = it->second;
        }
        if (t == IRType::UNKNOWN) {
            auto it = typeTable_.find(lastResult_);
            if (it != typeTable_.end()) t = it->second;
        }
        if (t == IRType::UNKNOWN) t = inferType(children[i]);
        argTypes.push_back(t);
    }

    IRType retType = IRType::UNKNOWN;

    for (auto& [decl, body] : program_.functions) {
        if (decl.name != funcName) continue;

        // --- Patch des types des paramètres ---
        for (size_t j = 0; j < decl.params.size() && j < argTypes.size(); ++j) {
            IRType t = argTypes[j];

            // Si toujours inconnu, cherche dans typeTable_ avec la valeur de l'arg
            if (t == IRType::UNKNOWN) {
                auto it = typeTable_.find(args[j]);
                if (it != typeTable_.end()) t = it->second;
            }

            if (t != IRType::UNKNOWN) {
                decl.params[j].first = t;
                typeTable_[decl.params[j].second] = t;
            }
        }

        // --- Patch des IR_Print dans le corps ---
        for (auto& instr : body.instructions) {
            if (std::holds_alternative<IR_Print>(instr)) {
                auto& p = std::get<IR_Print>(instr);
                if (p.type == IRType::UNKNOWN) {
                    for (const auto& [ptype, pname] : decl.params) {
                        if (pname == p.value && ptype != IRType::UNKNOWN) {
                            p.type = ptype;
                            break;
                        }
                    }
                }
            }
        }

        // --- Propage le type de retour si encore UNKNOWN ---
        if (decl.returnType == IRType::UNKNOWN) {
            for (const auto& instr : body.instructions) {
                if (std::holds_alternative<IR_Return>(instr)) {
                    const std::string& retVal = std::get<IR_Return>(instr).value;
                    // Cherche dans les instructions du corps
                    for (const auto& i2 : body.instructions) {
                        if (std::holds_alternative<IR_BinOp>(i2)) {
                            const auto& b = std::get<IR_BinOp>(i2);
                            if (b.dest == retVal && b.type != IRType::UNKNOWN) {
                                decl.returnType = b.type; break;
                            }
                        }
                        if (std::holds_alternative<IR_Call>(i2)) {
                            const auto& c = std::get<IR_Call>(i2);
                            if (c.dest == retVal && c.type != IRType::UNKNOWN) {
                                decl.returnType = c.type; break;
                            }
                        }
                        if (std::holds_alternative<IR_Assign>(i2)) {
                            const auto& a = std::get<IR_Assign>(i2);
                            if (a.dest == retVal) {
                                auto it = typeTable_.find(a.src);
                                if (it != typeTable_.end() && it->second != IRType::UNKNOWN) {
                                    decl.returnType = it->second; break;
                                }
                            }
                        }
                    }
                }
            }
        }

        retType = decl.returnType;
        break; // trouvé, inutile de continuer
    }

    // --- Détermine si la fonction est void ---
    bool isVoid = true;
    for (const auto& [decl, body] : program_.functions) {
        if (decl.name == funcName) {
            for (const auto& instr : body.instructions) {
                if (std::holds_alternative<IR_Return>(instr)) {
                    const auto& r = std::get<IR_Return>(instr);
                    if (!r.value.empty()) { isVoid = false; break; }
                }
            }
            break;
        }
    }

    if (isVoid) {
        emit(IR_Call{ IRType::VOID, "", funcName, args });
        return "";
    }

    IROperand dest = newTemp(retType);
    typeTable_[dest] = retType;
    emit(IR_Call{ retType, dest, funcName, args });
    return dest;
}

// =============================================================================
// Dump de l'IR (pour le debug)
// =============================================================================

void IRGenerator::dumpIR(const IRProgram& program, std::ostream& out) const {
    out << "=== IR DUMP ===\n\n";

    // Fonctions d'abord
    for (const auto& [decl, body] : program.functions) {
        out << "FUNCTION " << decl.name << "(";
        for (size_t i = 0; i < decl.params.size(); ++i) {
            out << irTypeToC(decl.params[i].first) << " " << decl.params[i].second;
            if (i + 1 < decl.params.size()) out << ", ";
        }
        out << ") -> " << irTypeToC(decl.returnType) << " {\n";
        dumpBlock(body, out);
        out << "}\n\n";
    }

    // Bloc principal
    out << "MAIN {\n";
    dumpBlock(program.mainBlock, out);
    out << "}\n";
}

void IRGenerator::dumpBlock(const IR_Block& block, std::ostream& out) const {
    for (const auto& instr : block.instructions) {
        out << "  " << instrToString(instr) << "\n";
    }
}

std::string IRGenerator::instrToString(const IRInstruction& instr) const {
    if (std::holds_alternative<IR_Assign>(instr)) {
        const auto& a = std::get<IR_Assign>(instr);
        return irTypeToC(a.type) + " " + a.dest + " = " + a.src + ";";
    }
    if (std::holds_alternative<IR_BinOp>(instr)) {
        const auto& b = std::get<IR_BinOp>(instr);
        return irTypeToC(b.type) + " " + b.dest + " = " + b.left + " " + b.op + " " + b.right + ";";
    }
    if (std::holds_alternative<IR_Call>(instr)) {
        const auto& c = std::get<IR_Call>(instr);
        std::string s = irTypeToC(c.type) + " " + c.dest + " = " + c.funcName + "(";
        for (size_t i = 0; i < c.args.size(); ++i) {
            s += c.args[i];
            if (i + 1 < c.args.size()) s += ", ";
        }
        return s + ");";
    }
    if (std::holds_alternative<IR_FuncDecl>(instr)) {
        return "/* func decl inline — should not appear in a block */";
    }
    if (std::holds_alternative<IR_CondJump>(instr)) {
        const auto& j = std::get<IR_CondJump>(instr);
        return "if (" + j.condition + ") goto " + j.labelTrue + " else goto " + j.labelFalse + ";";
    }
    if (std::holds_alternative<IR_Label>(instr)) {
        return std::get<IR_Label>(instr).name + ":";
    }
    if (std::holds_alternative<IR_Jump>(instr)) {
        return "goto " + std::get<IR_Jump>(instr).label + ";";
    }
    if (std::holds_alternative<IR_Return>(instr)) {
        return "return " + std::get<IR_Return>(instr).value + ";";
    }
    if (std::holds_alternative<IR_Print>(instr)) {
        const auto& p = std::get<IR_Print>(instr);
        return "PRINT(" + p.value + ");";
    }
    if (std::holds_alternative<IR_Scan>(instr)) {
        const auto& s = std::get<IR_Scan>(instr);
        return "SCAN(" + s.dest + ");";
    }
    return "/* unknown instruction */";
}
