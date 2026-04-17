//
// Created by kerya on 17/04/2026.
//

#ifndef COMPILATEUR_JULIESP_SEMANTICANALYZER_H
#define COMPILATEUR_JULIESP_SEMANTICANALYZER_H
#include "Visitor.h"
#include "SymbolTable.h"
#include "pch.h"


class SemanticAnalyzer : public Visitor {
private:
    Symtable symtable; // Contient la pile de Scopes

    //pour les stats
    int count_sexpr = 0;
    int count_symbols = 0;
    int count_primitives = 0;
    int count_literals = 0;

public:
    SemanticAnalyzer() = default;
    void analyze(ASTNode* root);

    // Export Graphviz de la table
    void dumpToDot(const std::string& filename);

    // --- S-Expressions ---
    void visit(SExpr* node) override;

    // --- Symboles & Primitives ---
    void visit(Identifier* node) override;
    void visit(Primitive* node) override;

    // --- Littéraux (On les visite pour valider le parcours) ---
    void visit(IntegerLit* node) override;
    void visit(FloatLit* node) override;
    void visit(CharLit* node) override;
    void visit(StringLit* node) override;
    void visit(BoolLit* node) override;

private:
    void printAllScopes(Scope* s, int indent);

};

#endif //COMPILATEUR_JULIESP_SEMANTICANALYZER_H