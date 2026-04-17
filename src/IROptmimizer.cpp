//
// Created by kerya on 17/04/2026.
//

#include "../include/IROptmimizer.h"


void IROptimizer::optimize(ControlFlowGraph& cfg) {
    eliminateDeadTemporaries(cfg);
    constantPropagation(cfg);
}

void IROptimizer::eliminateDeadTemporaries(ControlFlowGraph& cfg) {
    std::unordered_set<std::string> used;
    for (const auto& bb : cfg.blocks) {
        for (const auto& inst : bb->instructions) {
            if (!inst->arg1.empty() && inst->arg1[0] == '%')
                used.insert(inst->arg1);
            if (!inst->arg2.empty() && inst->arg2[0] == '%')
                used.insert(inst->arg2);
            for (const auto& arg : inst->args) {
                if (!arg.empty() && arg[0] == '%')
                    used.insert(arg);
            }
        }
    }
    for (auto& bb : cfg.blocks) {
        auto& insts = bb->instructions;
        insts.erase(std::remove_if(insts.begin(), insts.end(),
            [&](const std::shared_ptr<IRInstruction>& inst) {
                if ((inst->op == IROp::ADD || inst->op == IROp::SUB ||
                     inst->op == IROp::MUL || inst->op == IROp::DIV ||
                     inst->op == IROp::LT  || inst->op == IROp::GT  ||
                     inst->op == IROp::EQ  || inst->op == IROp::CALL) &&
                    !inst->dest.empty() && inst->dest[0] == '%') {
                    return used.find(inst->dest) == used.end();
                }
                return false;
            }), insts.end());
    }
}

void IROptimizer::constantPropagation(ControlFlowGraph& cfg) {
    std::unordered_map<std::string, std::string> constVars;
    for (auto& bb : cfg.blocks) {
        for (auto& inst : bb->instructions) {
            auto replace = [&](std::string& arg) {
                if (!arg.empty() && constVars.count(arg))
                    arg = constVars[arg];
            };
            replace(inst->arg1);
            replace(inst->arg2);
            for (auto& a : inst->args) replace(a);

            if (inst->op == IROp::ASSIGN) {
                if (inst->arg1.find_first_not_of("0123456789") == std::string::npos ||
                    inst->arg1 == "true" || inst->arg1 == "false") {
                    constVars[inst->dest] = inst->arg1;
                } else {
                    constVars.erase(inst->dest);
                }
            }
        }
    }
}