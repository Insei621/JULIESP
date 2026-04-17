// IRGenerator.cpp
#include "../include/IRGenerator.h"

// --------------------- Méthodes de Visitor (OBLIGATOIRES) ---------------------
void IRGenerator::visit(IntegerLit* node) {
    valueStack.push(std::to_string(node->value));
}
void IRGenerator::visit(FloatLit* node) {
    valueStack.push(std::to_string(node->value));
}
void IRGenerator::visit(CharLit* node) {
    valueStack.push("'" + std::string(1, node->value) + "'");
}
void IRGenerator::visit(StringLit* node) {
    valueStack.push("\"" + node->value + "\"");
}
void IRGenerator::visit(BoolLit* node) {
    valueStack.push(node->value ? "true" : "false");
}
void IRGenerator::visit(Identifier* node) {
    valueStack.push(node->getName());
}
void IRGenerator::visit(Primitive* node) {
    valueStack.push(node->getName());
}
void IRGenerator::visit(SExpr* node) {
    if (node->isQuotedNode()) {
        valueStack.push("'quoted");
        return;
    }
    const auto& children = node->getChildren();
    if (children.empty()) {
        valueStack.push("nil");
        return;
    }
    ASTNode* head = children[0];
    std::string op;
    if (auto* id = dynamic_cast<Identifier*>(head))
        op = id->getName();
    else if (auto* prim = dynamic_cast<Primitive*>(head))
        op = prim->getName();
    else {
        for (auto* child : children) child->accept(this);
        return;
    }

    if (op == "+" || op == "-" || op == "*" || op == "/" ||
        op == "<" || op == ">" || op == "=") {
        if (children.size() < 3) return;
        children[1]->accept(this);
        std::string lhs = valueStack.top(); valueStack.pop();
        children[2]->accept(this);
        std::string rhs = valueStack.top(); valueStack.pop();
        std::string result = handleBinaryOp(op, children[1], children[2]);
        valueStack.push(result);
        return;
    }
    if (op == "if" || op == "?") {
        handleIf(node);
        return;
    }
    if (op == "while") {
        handleWhile(node);
        return;
    }
    if (op == "setq" || op == ":") {
        handleSetq(node);
        return;
    }
    if (op == "progn" || op == ";") {
        handleProgn(node);
        return;
    }
    if (op == "lambda" || op == "£") {
        handleLambda(node);
        return;
    }
    if (op == "defun") {
        handleDefun(node);
        return;
    }
    std::string callResult = handleCall(node);
    if (!callResult.empty())
        valueStack.push(callResult);
}

// --------------------- Fonctions auxiliaires ---------------------
std::string IRGenerator::handleBinaryOp(const std::string& op, ASTNode* lhs, ASTNode* rhs) {
    std::string rhsVal = valueStack.top(); valueStack.pop();
    std::string lhsVal = valueStack.top(); valueStack.pop();
    std::string tmp = newTmp();
    IROp irOp;
    if (op == "+") irOp = IROp::ADD;
    else if (op == "-") irOp = IROp::SUB;
    else if (op == "*") irOp = IROp::MUL;
    else if (op == "/") irOp = IROp::DIV;
    else if (op == "<") irOp = IROp::LT;
    else if (op == ">") irOp = IROp::GT;
    else if (op == "=") irOp = IROp::EQ;
    else throw std::runtime_error("Opérateur inconnu");
    emit(irOp, tmp, lhsVal, rhsVal);
    return tmp;
}

void IRGenerator::handleIf(SExpr* node) {
    const auto& children = node->getChildren();
    if (children.size() < 3) return;
    children[1]->accept(this);
    std::string cond = valueStack.top(); valueStack.pop();

    auto thenBlock = createBlock(newLabel("if_then"));
    auto elseBlock = createBlock(newLabel("if_else"));
    auto mergeBlock = createBlock(newLabel("if_merge"));

    emit(IROp::ITE, thenBlock->label, cond, elseBlock->label);
    addEdge(currentBlock, thenBlock);
    addEdge(currentBlock, elseBlock);

    startBlock(thenBlock);
    children[2]->accept(this);
    if (!valueStack.empty()) valueStack.pop();
    emit(IROp::GOTO, mergeBlock->label);
    addEdge(thenBlock, mergeBlock);

    startBlock(elseBlock);
    if (children.size() > 3) {
        children[3]->accept(this);
        if (!valueStack.empty()) valueStack.pop();
    }
    emit(IROp::GOTO, mergeBlock->label);
    addEdge(elseBlock, mergeBlock);

    startBlock(mergeBlock);
    valueStack.push("");
}

void IRGenerator::handleWhile(SExpr* node) {
    const auto& children = node->getChildren();
    if (children.size() < 3) return;

    auto headerBlock = createBlock(newLabel("while_header"));
    auto bodyBlock = createBlock(newLabel("while_body"));
    auto exitBlock = createBlock(newLabel("while_exit"));

    emit(IROp::GOTO, headerBlock->label);
    addEdge(currentBlock, headerBlock);

    startBlock(headerBlock);
    children[1]->accept(this);
    std::string cond = valueStack.top(); valueStack.pop();
    emit(IROp::ITE, bodyBlock->label, cond, exitBlock->label);
    addEdge(headerBlock, bodyBlock);
    addEdge(headerBlock, exitBlock);

    startBlock(bodyBlock);
    children[2]->accept(this);
    if (!valueStack.empty()) valueStack.pop();
    emit(IROp::GOTO, headerBlock->label);
    addEdge(bodyBlock, headerBlock);

    startBlock(exitBlock);
    valueStack.push("");
}

void IRGenerator::handleSetq(SExpr* node) {
    const auto& children = node->getChildren();
    if (children.size() < 3) return;
    Identifier* var = dynamic_cast<Identifier*>(children[1]);
    if (!var) return;
    children[2]->accept(this);
    std::string val = valueStack.top(); valueStack.pop();
    emit(IROp::ASSIGN, var->getName(), val);
    valueStack.push(var->getName());
}

void IRGenerator::handleProgn(SExpr* node) {
    const auto& children = node->getChildren();
    std::string lastValue;
    for (size_t i = 1; i < children.size(); ++i) {
        children[i]->accept(this);
        if (!valueStack.empty()) {
            lastValue = valueStack.top();
            if (i < children.size() - 1) valueStack.pop();
        }
    }
    valueStack.push(lastValue);
}

void IRGenerator::handleLambda(SExpr* node) {
    std::string lambdaLabel = newLabel("lambda");
    valueStack.push(lambdaLabel);
}

std::string IRGenerator::handleCall(SExpr* node) {
    const auto& children = node->getChildren();
    if (children.empty()) return "";
    ASTNode* head = children[0];
    std::string funcName;
    if (auto* id = dynamic_cast<Identifier*>(head))
        funcName = id->getName();
    else if (auto* prim = dynamic_cast<Primitive*>(head))
        funcName = prim->getName();
    else return "";

    std::vector<std::string> argValues;
    for (size_t i = 1; i < children.size(); ++i) {
        children[i]->accept(this);
        argValues.push_back(valueStack.top());
        valueStack.pop();
    }

    static const std::set<std::string> knownPrimitives = {
        "car", "<<", "cdr", ">>", "cons", "&",
        "null", "|", "atom", "@", "print", "€", "load", "$", "scan", "ç"
    };

    if (knownPrimitives.count(funcName) || dynamic_cast<Primitive*>(head)) {
        std::string result = newTmp();
        auto inst = std::make_shared<IRInstruction>(IROp::CALL, result, funcName, argValues);
        emit(inst);
        return result;
    } else {
        std::cerr << "[IRGen] Avertissement: appel à fonction utilisateur '"
                  << funcName << "' ignoré.\n";
        return "";
    }
}

// --------------------- IRGenerator::generateModule (définition complète) ---------------------
IRModule IRGenerator::generateModule(ASTNode* root) {
    module = IRModule();
    currentCFG = &module.mainCFG;

    currentCFG->entry = createBlock(newLabel("entry"));
    startBlock(currentCFG->entry);

    root->accept(this);

    currentCFG->exit = createBlock(newLabel("exit"));
    addEdge(currentBlock, currentCFG->exit);
    startBlock(currentCFG->exit);
    emit(IROp::RETURN, "", "0");

    return module;
}

// --------------------- IRGenerator::handleDefun ---------------------
void IRGenerator::handleDefun(SExpr* node) {
    const auto& children = node->getChildren();
    if (children.size() < 4) return;

    Identifier* nameId = dynamic_cast<Identifier*>(children[1]);
    if (!nameId) return;
    std::string funcName = nameId->getName();

    // Sauvegarde du contexte actuel
    ControlFlowGraph* savedCFG = currentCFG;
    std::shared_ptr<BasicBlock> savedBlock = currentBlock;

    // Création d'un CFG temporaire pour la fonction
    ControlFlowGraph funcCFG;
    currentCFG = &funcCFG;
    funcCFG.entry = createBlock(newLabel(funcName + "_entry"));
    startBlock(funcCFG.entry);

    // Traitement des paramètres (optionnel)
    if (auto* paramList = dynamic_cast<SExpr*>(children[2])) {
        for (auto* param : paramList->getChildren()) {
            if (auto* pid = dynamic_cast<Identifier*>(param)) {
                // Rien pour l'instant
            }
        }
    }

    // Visite du corps de la fonction
    children[3]->accept(this);

    // Valeur de retour par défaut
    std::string retVal = "0";
    if (!valueStack.empty()) {
        retVal = valueStack.top();
        valueStack.pop();
    }
    emit(IROp::RETURN, "", retVal);
    funcCFG.exit = currentBlock;

    // Stockage de la fonction dans le module
    module.functions[funcName] = funcCFG;

    // Restauration du contexte principal
    currentCFG = savedCFG;
    currentBlock = savedBlock;
}

// --------------------- IRGenerator::generate (pour compatibilité) ---------------------
ControlFlowGraph IRGenerator::generate(ASTNode* root) {
    IRModule mod = generateModule(root);
    return mod.mainCFG;
}

// --------------------- IROptimizer ---------------------
// (vos définitions existantes restent inchangées)