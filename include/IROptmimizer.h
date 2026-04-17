#ifndef COMPILATEUR_JULIESP_IROPTIMIZER_H
#define COMPILATEUR_JULIESP_IROPTIMIZER_H

#include "IR.h"
#include "pch.h"

class IROptimizer {
public:
    void optimize(ControlFlowGraph& cfg);

private:
    void eliminateDeadTemporaries(ControlFlowGraph& cfg);
    void constantPropagation(ControlFlowGraph& cfg);
};

#endif