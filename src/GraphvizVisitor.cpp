//
// Created by kerya on 17/04/2026.
//

#include "../include/GraphvizVisitor.h"
#include "../include/AST.h"

GraphvizVisitor::GraphvizVisitor(std::ofstream& os) : out(os) {}

std::string GraphvizVisitor::getID(void* ptr) {
    std::stringstream ss;
    ss << "node_" << reinterpret_cast<uintptr_t>(ptr);
    return ss.str();
}

void GraphvizVisitor::visit(SExpr* node) {
    std::string id = getID(node);

    // Style pour la S-Expression
    std::string label = node->isQuotedNode() ? "SExpr (²)" : "SExpr";
    out << "  " << id << " [label=\"" << label << "\", shape=ellipse, style=filled, fillcolor=lightgrey];\n";

    for (auto child : node->getChildren()) {
        if (child) {
            out << "  " << id << " -> " << getID(child) << ";\n";
            child->accept(this);
        }
    }
}

void GraphvizVisitor::visit(Identifier* node) {
    out << "  " << getID(node) << " [label=\"ID: " << node->getName() << "\", shape=box, color=green];\n";
}

void GraphvizVisitor::visit(Primitive* node) {
    out << "  " << getID(node) << " [label=\"PRIM: " << node->getName() << "\", shape=diamond, color=red];\n";
}

void GraphvizVisitor::visit(IntegerLit* node) {
    out << "  " << getID(node) << " [label=\"" << node->value << "\", shape=circle, color=blue];\n";
}

// Implémentation des autres pour ne plus être "abstract"
void GraphvizVisitor::visit(FloatLit* node) {
    out << "  " << getID(node) << " [label=\"" << node->value << "\", shape=circle];\n";
}

void GraphvizVisitor::visit(StringLit* node) {
    out << "  " << getID(node) << " [label=\"\\\"" << node->value << "\\\"\", shape=note];\n";
}

void GraphvizVisitor::visit(BoolLit* node) {
    out << "  " << getID(node) << " [label=\"" << (node->value ? "µ" : "ù") << "\", shape=diamond];\n";
}

void GraphvizVisitor::visit(CharLit* node) {
    out << "  " << getID(node) << " [label=\"'" << node->value << "'\"];\n";
}