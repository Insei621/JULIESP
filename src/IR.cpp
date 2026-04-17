//
// Created by kerya on 17/04/2026.
//

#include "../include/IR.h"

std::string IRInstruction::toString() const {
    std::stringstream ss;
    switch (op) {
        case IROp::ADD:  ss << dest << " = " << arg1 << " + " << arg2; break;
        case IROp::SUB:  ss << dest << " = " << arg1 << " - " << arg2; break;
        case IROp::MUL:  ss << dest << " = " << arg1 << " * " << arg2; break;
        case IROp::DIV:  ss << dest << " = " << arg1 << " / " << arg2; break;
        case IROp::LT:   ss << dest << " = " << arg1 << " < " << arg2; break;
        case IROp::GT:   ss << dest << " = " << arg1 << " > " << arg2; break;
        case IROp::EQ:   ss << dest << " = " << arg1 << " == " << arg2; break;
        case IROp::ASSIGN: ss << dest << " = " << arg1; break;
        case IROp::GOTO:   ss << "goto " << dest; break;
        case IROp::ITE:    ss << "if " << arg1 << " goto " << dest << " else " << arg2; break;
        case IROp::LABEL:  ss << dest << ":"; break;
        case IROp::CALL:
            ss << dest << " = call " << arg1;
            if (!args.empty()) {
                ss << "(";
                for (size_t i = 0; i < args.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << args[i];
                }
                ss << ")";
            }
            break;
        case IROp::RETURN: ss << "return " << arg1; break;
        default: ss << "unknown"; break;
    }
    return ss.str();
}

void ControlFlowGraph::print(std::ostream& out) const {
    for (const auto& bb : blocks) {
        out << "\n=== Block " << bb->label << " ===" << std::endl;
        out << "Predecessors: ";
        for (const auto* pred : bb->predecessors) {
            out << pred->label << " ";
        }
        out << "\nInstructions:" << std::endl;
        for (const auto& inst : bb->instructions) {
            out << "  " << inst->toString() << std::endl;
        }
        out << "Successors: ";
        for (const auto& succ : bb->successors) {
            out << succ->label << " ";
        }
        out << std::endl;
    }
}

void IRModule::print(std::ostream& out) const {
    out << "=== Main Function ===\n";
    mainCFG.print(out);
    for (const auto& [name, cfg] : functions) {
        out << "\n=== Function " << name << " ===\n";
        cfg.print(out);
    }
}