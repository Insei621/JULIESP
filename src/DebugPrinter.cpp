//
// Created by kerya on 10/05/2026.
//

#include "../include/DebugPrinter.h"
#include <iostream>

void DebugPrinter::dumpLex(Lexer& lexer, const std::vector<Token>& tokens) {
    std::cout << "\n=== DUMP LEX ===\n";
    lexer.afficherTokens(tokens);
    std::cout << "\n";
}

void DebugPrinter::dumpAst(ASTNode* root) {
    std::cout << "\n=== DUMP AST ===\n";
    PrettyPrinter printer;
    root->accept(&printer);
    std::cout << "\n";
}

void DebugPrinter::dumpImgAst(ASTNode* root, const std::string& outputFile) {
    auto outDir = std::filesystem::path(outputFile).parent_path();
    if (outDir.empty()) outDir = ".";
    std::filesystem::create_directories(outDir);

    std::string dotPath = (outDir / "ast.dot").string();
    std::string pngPath = (outDir / "ast.png").string();

    std::ofstream dotFile(dotPath);
    dotFile << "digraph G {\n";
    GraphvizVisitor gv(dotFile);
    root->accept(&gv);
    dotFile << "}\n";
    dotFile.close();

    std::string cmd = "dot -Tpng \"" + dotPath + "\" -o \"" + pngPath + "\"";
    int ret = system(cmd.c_str());
    if (ret != 0)
        std::cerr << "\033[1;33m[Attention]\033[0m graphviz a échoué — est-il installé ?\n";
    else
        std::cout << "\033[1;32m[juliesp]\033[0m Image AST : " << pngPath << "\n";
}

void DebugPrinter::dumpIR(IRGenerator& irGen, const IRProgram& ir) {
    std::cout << "\n=== DUMP IR ===\n";
    irGen.dumpIR(ir);
    std::cout << "\n";
}

void DebugPrinter::logStart(const std::string& etape) {
    std::cout << "\033[1;34m[" << etape << "]\033[0m Lancement de l'analyse...\n";
}

void DebugPrinter::logSuccess(const std::string& message) {
    std::cout << "\033[1;32m[Succès]\033[0m " << message << "\n";
}

void DebugPrinter::logInfo(const std::string& message) {
    std::cout << "\033[1;34m[juliesp]\033[0m " << message << "\n";
}

void DebugPrinter::logWarning(const std::string& message) {
    std::cerr << "\033[1;33m[Attention]\033[0m " << message << "\n";
}

void DebugPrinter::logError(const std::string& message) {
    std::cerr << "\033[1;31m[Erreur]\033[0m " << message << "\n";
}