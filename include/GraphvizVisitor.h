//
// Created by kerya on 17/04/2026.
//

#ifndef COMPILATEUR_JULIESP_GRAPHVIZVISITOR_H
#define COMPILATEUR_JULIESP_GRAPHVIZVISITOR_H

#include "../include/Visitor.h"
#include "pch.h"

class GraphvizVisitor : public Visitor {
private:
    std::ofstream& out;
    // Utilitaire pour obtenir un ID unique pour Graphviz
    std::string getID(void* ptr);

public:
    explicit GraphvizVisitor(std::ofstream& os);

    // Tu DOIS redéfinir TOUTES les méthodes virtuelles pures de Visitor
    void visit(SExpr* node) override;
    void visit(Identifier* node) override;
    void visit(Primitive* node) override;
    void visit(IntegerLit* node) override;
    void visit(FloatLit* node) override;
    void visit(StringLit* node) override;
    void visit(BoolLit* node) override;
    void visit(CharLit* node) override;
};

#endif
