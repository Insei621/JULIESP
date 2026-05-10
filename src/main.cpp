//
// main.cpp — Point d'entrée du compilateur JULIESP
//

#include "../include/CompilerConfig.h"
#include "../include/DebugPrinter.h"
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/SemanticAnalyzer.h"
#include "../include/IRGenerator.h"
#include "../include/CGenerator.h"

#include <fstream>
#include <sstream>
#include <filesystem>

std::string readSourceFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        DebugPrinter::logError("Impossible d'ouvrir le fichier : " + path);
        exit(1);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {

    // --- Configuration ---
    CompilerConfig cfg;
    if (!CompilerConfig::parse(argc, argv, cfg)) return 2;

    DebugPrinter::logInfo("Compilation de : " + cfg.sourceFile);

    // --- Lexage ---
    DebugPrinter::logStart("Lexage");
    Lexer lexer(readSourceFile(cfg.sourceFile));
    auto tokens = lexer.tokenize();
    if (cfg.dumpLex) DebugPrinter::dumpLex(lexer, tokens);
    DebugPrinter::logSuccess("Analyse lexicale terminée.");
    if (cfg.lexOnly) return 0;

    // --- Parsing ---
    DebugPrinter::logStart("Parsing");
    std::string sourceDir = std::filesystem::path(cfg.sourceFile).parent_path().string();
    Parser parser(tokens, sourceDir);
    ASTNode* root = nullptr;
    try {
        auto programNodes = parser.parseProgram();
        SExpr* rootExpr = new SExpr(0, 0, false);
        for (ASTNode* node : programNodes) rootExpr->add(node);
        root = rootExpr;
        if (cfg.dumpAst)    DebugPrinter::dumpAst(root);
        if (cfg.dumpImgAst) DebugPrinter::dumpImgAst(root, cfg.outputFile);
        DebugPrinter::logSuccess("Analyse syntaxique terminée ("
            + std::to_string(programNodes.size()) + " expressions).");
    } catch (const std::exception& e) {
        DebugPrinter::logError(std::string("Erreur Syntaxique : ") + e.what());
        return 1;
    }
    if (cfg.parseOnly) return 0;

    // --- Sémantique ---
    DebugPrinter::logStart("Sémantique");
    try {
        SemanticAnalyzer semantic;
        semantic.analyze(root);
        DebugPrinter::logSuccess("Analyse sémantique terminée.");
    } catch (const std::exception& e) {
        DebugPrinter::logError(std::string("Erreur Sémantique : ") + e.what());
        return 1;
    }
    if (cfg.semOnly) return 0;

    // --- Génération IR ---
    DebugPrinter::logStart("IR");
    IRGenerator irGen;
    IRProgram ir = irGen.generate(root);
    if (cfg.dumpIR) DebugPrinter::dumpIR(irGen, ir);
    DebugPrinter::logSuccess("IR générée.");
    if (cfg.irOnly) return 0;

    // --- Génération C ---
    DebugPrinter::logStart("CGen");
    try {
        auto outPath = std::filesystem::path(cfg.outputFile);
        if (outPath.has_parent_path())
            std::filesystem::create_directories(outPath.parent_path());
        CGenerator cGen;
        cGen.generateToFile(ir, cfg.outputFile);
        DebugPrinter::logInfo("Code C généré : " + cfg.outputFile);
    } catch (const std::exception& e) {
        DebugPrinter::logError(e.what());
        return 1;
    }

    // --- Compilation gcc ---
    if (cfg.compileOutput) {
        std::string binFile = cfg.binaryName.empty()
            ? cfg.outputFile.substr(0, cfg.outputFile.size() - 2)
            : cfg.binaryName;

        DebugPrinter::logInfo("Compilation gcc...");
        int ret = system(("gcc \"" + cfg.outputFile + "\" -o \"" + binFile + "\" -I/usr/local/include 2>&1").c_str());
        if (ret != 0) { DebugPrinter::logError("Échec de la compilation gcc."); return 1; }
        DebugPrinter::logInfo("Binaire généré : " + binFile);

        // --- Exécution ---
        if (cfg.runOutput) {
            DebugPrinter::logInfo("Exécution de : " + binFile);
            std::cout << std::string(40, '-') << "\n";
            int runRet = system(("\"" + binFile + "\"").c_str());
            std::cout << std::string(40, '-') << "\n";
            if (runRet != 0)
                DebugPrinter::logWarning("Programme terminé avec le code " + std::to_string(runRet));
        }
    }

    delete root;
    return 0;
}