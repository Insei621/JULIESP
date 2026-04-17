//
// Created by kerya on 16/04/2026.
//

#ifndef COMPILATEUR_JULIESP_PRETTYPRINTER_H
#define COMPILATEUR_JULIESP_PRETTYPRINTER_H
//
// Created by kerya on 16/04/2026.
//

#include "../include/Visitor.h"
#include "AST.h"
#include "pch.h"

class PrettyPrinter : public Visitor {
public:
    PrettyPrinter();
    ~PrettyPrinter() override = default;

    // --- Méthodes de visite pour chaque nœud de l'AST ---
    void visit(class SExpr* node) override;
    void visit(class Identifier* node) override;
    void visit(class Primitive* node) override;
    void visit(class IntegerLit* node) override;
    void visit(class FloatLit* node) override;
    void visit(class StringLit* node) override;
    void visit(class BoolLit* node) override;
    void visit(class CharLit* node) override;

private:
    int indentLevel = 0;

    // Méthodes utilitaires pour le formatage
    void printIndent();
    void newLine();
};

#endif //COMPILATEUR_JULIESP_PRETTYPRINTER_H

