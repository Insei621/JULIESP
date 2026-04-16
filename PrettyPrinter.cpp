//
// Created by kerya on 16/04/2026.
//

#include "PrettyPrinter.h"


#include "pch.h"
#include "PrettyPrinter.h"

// Initialisation du constructeur (si nécessaire)
PrettyPrinter::PrettyPrinter() : indentLevel(0) {}

void PrettyPrinter::printIndent() {
    std::cout << std::string(indentLevel * 2, ' ');
}

// --- Gestion des listes (S-Expressions) ---
void PrettyPrinter::visit(SExpr* node) {
    std::cout << "("; // Les S-Expr utilisent généralement des parenthèses

    const auto& children = node->getChildren(); // Utilisation du getter
    if (children.empty()) {
        std::cout << ")";
        return;
    }

    std::cout << "\n";
    indentLevel++;
    for (size_t i = 0; i < children.size(); ++i) {
        printIndent();
        if (children[i]) {
            children[i]->accept(this);
        } else {
            std::cout << "nil";
        }

        if (i < children.size() - 1) std::cout << "\n";
    }
    indentLevel--;
    std::cout << "\n";
    printIndent();
    std::cout << ")";
}

// --- Symboles ---
// Note: On utilise getName() car 'name' est protected dans ASTNode
void PrettyPrinter::visit(Identifier* node) {
    std::cout << node->getName();
}

void PrettyPrinter::visit(Primitive* node) {
    std::cout << node->getName();
}

// --- Littéraux ---
void PrettyPrinter::visit(IntegerLit* node) { std::cout << node->value; }
void PrettyPrinter::visit(FloatLit* node)   { std::cout << node->value; }
void PrettyPrinter::visit(BoolLit* node)    { std::cout << (node->value ? "true" : "false"); }
void PrettyPrinter::visit(CharLit* node)    { std::cout << "'" << node->value << "'"; }
void PrettyPrinter::visit(StringLit* node)  { std::cout << "\"" << node->value << "\""; }