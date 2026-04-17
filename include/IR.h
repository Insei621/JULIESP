#ifndef COMPILATEUR_JULIESP_IR_H
#define COMPILATEUR_JULIESP_IR_H

#include "pch.h"

enum class IROp {
    ADD, SUB, MUL, DIV,
    LT, GT, LE, GE, EQ, NE,
    ASSIGN,
    GOTO,
    ITE,
    LABEL,
    CALL,
    RETURN
};

class IRInstruction {
public:
    IROp op;
    std::string dest;
    std::string arg1;
    std::string arg2;
    std::vector<std::string> args;

    IRInstruction(IROp o, const std::string& d = "",
                  const std::string& a1 = "", const std::string& a2 = "")
        : op(o), dest(d), arg1(a1), arg2(a2) {}

    IRInstruction(IROp o, const std::string& d, const std::string& func,
                  const std::vector<std::string>& a)
        : op(o), dest(d), arg1(func), args(a) {}

    std::string toString() const;
};

class BasicBlock {
public:
    std::string label;
    std::vector<std::shared_ptr<IRInstruction>> instructions;
    std::vector<std::shared_ptr<BasicBlock>> successors;
    std::vector<BasicBlock*> predecessors;

    BasicBlock(const std::string& lbl) : label(lbl) {}

    void addInstruction(std::shared_ptr<IRInstruction> inst) {
        instructions.push_back(inst);
    }

    void addSuccessor(std::shared_ptr<BasicBlock> succ) {
        successors.push_back(succ);
        succ->predecessors.push_back(this);
    }
};

class ControlFlowGraph {
public:
    std::vector<std::shared_ptr<BasicBlock>> blocks;
    std::shared_ptr<BasicBlock> entry;
    std::shared_ptr<BasicBlock> exit;

    void addBlock(std::shared_ptr<BasicBlock> block) {
        blocks.push_back(block);
    }

    void print(std::ostream& out = std::cout) const;
};

class IRModule {
public:
    ControlFlowGraph mainCFG;
    std::unordered_map<std::string, ControlFlowGraph> functions;

    void print(std::ostream& out = std::cout) const;
};

#endif