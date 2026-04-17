#ifndef COMPILATEUR_JULIESP_IRGENERATOR_H
#define COMPILATEUR_JULIESP_IRGENERATOR_H

#include "pch.h"
#include "AST.h"
#include "Visitor.h"
#include "IR.h"


class IRGenerator : public Visitor {
private:
    IRModule module;
    ControlFlowGraph* currentCFG = nullptr;   // pointeur vers le CFG actif (mainCFG ou une fonction)
    std::shared_ptr<BasicBlock> currentBlock;
    std::stack<std::string> valueStack;

    int tmpCounter = 0;
    int labelCounter = 0;

    std::string newTmp() { return "%t" + std::to_string(++tmpCounter); }
    std::string newLabel(const std::string& prefix = "L") {
        return prefix + std::to_string(++labelCounter);
    }

    void emit(std::shared_ptr<IRInstruction> inst) {
        if (currentBlock) currentBlock->addInstruction(inst);
    }

    void emit(IROp op, const std::string& d = "",
              const std::string& a1 = "", const std::string& a2 = "") {
        emit(std::make_shared<IRInstruction>(op, d, a1, a2));
    }

    std::shared_ptr<BasicBlock> createBlock(const std::string& label) {
        auto block = std::make_shared<BasicBlock>(label);
        if (currentCFG) currentCFG->addBlock(block);
        return block;
    }

    void startBlock(std::shared_ptr<BasicBlock> block) {
        currentBlock = block;
    }

    void addEdge(std::shared_ptr<BasicBlock> from, std::shared_ptr<BasicBlock> to) {
        from->addSuccessor(to);
    }

    // Formes spéciales
    void handleIf(SExpr* node);
    void handleWhile(SExpr* node);
    void handleSetq(SExpr* node);
    void handleProgn(SExpr* node);
    void handleLambda(SExpr* node);
    void handleDefun(SExpr* node);
    std::string handleCall(SExpr* node);
    std::string handleBinaryOp(const std::string& op, ASTNode* lhs, ASTNode* rhs);

public:
    IRGenerator() = default;

    IRModule generateModule(ASTNode* root);
    ControlFlowGraph generate(ASTNode* root); // obsolète mais conservé pour compatibilité

    void visit(IntegerLit* node) override;
    void visit(FloatLit* node) override;
    void visit(CharLit* node) override;
    void visit(StringLit* node) override;
    void visit(BoolLit* node) override;
    void visit(Identifier* node) override;
    void visit(Primitive* node) override;
    void visit(SExpr* node) override;
};

#endif