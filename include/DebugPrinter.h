//
// Created by kerya on 10/05/2026.
//

#ifndef JULIESP_DEBUGPRINTER_H
#define JULIESP_DEBUGPRINTER_H

#include "Lexer.h"
#include "IRGenerator.h"
#include "GraphvizVisitor.h"
#include "PrettyPrinter.h"
#include "pch.h"

class DebugPrinter {
public:
    // Affiche les tokens
    static void dumpLex(Lexer& lexer, const std::vector<Token>& tokens);

    // Affiche l'AST dans le terminal
    static void dumpAst(ASTNode* root);

    // Génère l'image AST
    static void dumpImgAst(ASTNode* root, const std::string& outputFile);

    // Affiche l'IR
    static void dumpIR(IRGenerator& irGen, const IRProgram& ir);
};

#endif //JULIESP_DEBUGPRINTER_H