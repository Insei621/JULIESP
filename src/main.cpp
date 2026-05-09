//
// main.cpp — Point d'entrée du compilateur JuliesSP
//

#include "../include/pch.h"
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/PrettyPrinter.h"
#include "../include/GraphvizVisitor.h"
#include "../include/SemanticAnalyzer.h"
#include "../include/IRGenerator.h"
#include "../include/CGenerator.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <filesystem>

// =============================================================================
// Affichage de l'aide
// =============================================================================

void printHelp(const char* progName) {
    std::cout << "\n";
    std::cout << "  juliesp — Compilateur du langage JuliesSP\n";
    std::cout << "\n";
    std::cout << "Usage:\n";
    std::cout << "  " << progName << " <fichier.jlsp> [options]\n";
    std::cout << "\n";
    std::cout << "Options de sortie:\n";
    std::cout << "  -o, --output <fichier.c>   Fichier de sortie C (défaut: output/output.c)\n";
    std::cout << "  -c, --compile              Compile le C généré avec gcc\n";
    std::cout << "\n";
    std::cout << "Options de debug:\n";
    std::cout << "  -dl, --dump-lex            Affiche les tokens\n";
    std::cout << "  -da, --dump-ast            Affiche l'AST dans le terminal\n";
    std::cout << "  -di, --dump-imgast         Génère AST_Graphe/ast.png\n";
    std::cout << "  -dr, --dump-ir             Affiche la représentation intermédiaire\n";
    std::cout << "\n";
    std::cout << "Options de pipeline:\n";
    std::cout << "  --lex-only                 S'arrête après le lexage\n";
    std::cout << "  --parse-only               S'arrête après le parsing\n";
    std::cout << "  --sem-only                 S'arrête après l'analyse sémantique\n";
    std::cout << "  --ir-only                  S'arrête après la génération IR\n";
    std::cout << "\n";
    std::cout << "  -h, --help                 Affiche cette aide\n";
    std::cout << "\n";
    std::cout << "Exemples:\n";
    std::cout << "  " << progName << " programme.jlsp\n";
    std::cout << "  " << progName << " programme.jlsp -o mon_prog.c\n";
    std::cout << "  " << progName << " programme.jlsp -c\n";
    std::cout << "  " << progName << " programme.jlsp -da -dr\n";
    std::cout << "  " << progName << " programme.jlsp -di\n";
    std::cout << "\n";
}

// =============================================================================
// Lecture d'un fichier source
// =============================================================================

std::string readSourceFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "\033[1;31m[Erreur]\033[0m Impossible d'ouvrir le fichier : "
                  << path << "\n";
        exit(1);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// =============================================================================
// Point d'entrée
// =============================================================================

int main(int argc, char** argv) {

    // --- Aide ---
    if (argc < 2) {
        std::cerr << "Usage: juliesp <fichier.jlsp> [options]\n";
        std::cerr << "Utilisez -h pour l'aide.\n";
        return 2;
    }
    if (std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0) {
        printHelp(argv[0]);
        return 0;
    }

    // --- Parsing des arguments ---
    std::string sourceFile  = argv[1];
    std::string outputFile  = "output/output.c";
    bool dumpLex       = false;
    bool dumpAst       = false;
    bool dumpImgAst    = false;
    bool dumpIR        = false;
    bool lexOnly       = false;
    bool parseOnly     = false;
    bool semOnly       = false;
    bool irOnly        = false;
    bool compileOutput = false;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];

        // Sortie
        if (arg == "-o" || arg == "--output") {
            if (i + 1 >= argc) {
                std::cerr << "\033[1;31m[Erreur]\033[0m "
                          << arg << " attend un nom de fichier.\n";
                return 2;
            }
            outputFile = argv[++i];
        }
        else if (arg == "-c"  || arg == "--compile")     compileOutput = true;

        // Debug
        else if (arg == "-dl" || arg == "--dump-lex")    dumpLex       = true;
        else if (arg == "-da" || arg == "--dump-ast")    dumpAst       = true;
        else if (arg == "-di" || arg == "--dump-imgast") dumpImgAst    = true;
        else if (arg == "-dr" || arg == "--dump-ir")     dumpIR        = true;

        // Pipeline
        else if (arg == "--lex-only")                    lexOnly       = true;
        else if (arg == "--parse-only")                  parseOnly     = true;
        else if (arg == "--sem-only")                    semOnly       = true;
        else if (arg == "--ir-only")                     irOnly        = true;

        else {
            std::cerr << "\033[1;31m[Erreur]\033[0m Option inconnue : " << arg << "\n";
            std::cerr << "Utilisez -h pour voir les options disponibles.\n";
            return 2;
        }
    }

    std::cout << "\033[1;34m[juliesp]\033[0m Compilation de : " << sourceFile << "\n";

    // ==========================================================================
    // ÉTAPE 1 : Lecture du fichier source
    // ==========================================================================

    std::string source = readSourceFile(sourceFile);

    // ==========================================================================
    // ÉTAPE 2 : Analyse lexicale
    // ==========================================================================

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    std::cout << "\033[1;32m[Succès]\033[0m Analyse lexicale terminée.\n";

    if (dumpLex) {
        lexer.afficherTokens(tokens);
    }

    if (lexOnly) return 0;

    // ==========================================================================
    // ÉTAPE 3 : Analyse syntaxique
    // ==========================================================================

    std::string sourceDir = std::filesystem::path(sourceFile)
                            .parent_path()
                            .string();

    Parser parser(tokens, sourceDir);
    ASTNode* root = nullptr;

    try {
        std::vector<ASTNode*> programNodes = parser.parseProgram();
        SExpr* rootExpr = new SExpr(0, 0, false);
        for (ASTNode* node : programNodes)
            rootExpr->add(node);
        root = rootExpr;

        std::cout << "\033[1;32m[Succès]\033[0m Analyse syntaxique terminée ("
                  << programNodes.size() << " expressions).\n";
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m[Erreur Syntaxique]\033[0m " << e.what() << "\n";
        return 1;
    }

    if (dumpAst) {
        std::cout << "\n=== DUMP AST ===\n";
        PrettyPrinter printer;
        root->accept(&printer);
        std::cout << "\n";
    }

    if (dumpImgAst) {
        // Place l'image dans le même dossier que le fichier de sortie
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
        int dotRet = system(cmd.c_str());
        if (dotRet != 0)
            std::cerr << "\033[1;33m[Attention]\033[0m graphviz a échoué — est-il installé ?\n";
        std::cout << "\033[1;32m[juliesp]\033[0m Image AST : " << pngPath << "\n";
    }

    if (parseOnly) return 0;

    // ==========================================================================
    // ÉTAPE 4 : Analyse sémantique
    // ==========================================================================

    SemanticAnalyzer semantic;
    try {
        semantic.analyze(root);
        std::cout << "\033[1;32m[Succès]\033[0m Analyse sémantique terminée.\n";
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m[Erreur Sémantique]\033[0m " << e.what() << "\n";
        return 1;
    }

    if (semOnly) return 0;

    // ==========================================================================
    // ÉTAPE 5 : Génération IR
    // ==========================================================================

    IRGenerator irGen;
    IRProgram ir = irGen.generate(root);

    if (dumpIR) {
        std::cout << "\n";
        irGen.dumpIR(ir);
        std::cout << "\n";
    }

    if (irOnly) return 0;

    // ==========================================================================
    // ÉTAPE 6 : Génération C
    // ==========================================================================

    try {
        auto outPath = std::filesystem::path(outputFile);
        if (outPath.has_parent_path())
            std::filesystem::create_directories(outPath.parent_path());

        CGenerator cGen;
        cGen.generateToFile(ir, outputFile);
        std::cout << "\033[1;32m[juliesp]\033[0m Code C généré : " << outputFile << "\n";
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m[Erreur]\033[0m " << e.what() << "\n";
        return 1;
    }

    // ==========================================================================
    // ÉTAPE 7 (optionnelle) : Compilation gcc
    // ==========================================================================

    if (compileOutput) {
        std::string binFile = outputFile;
        if (binFile.size() > 2 && binFile.substr(binFile.size() - 2) == ".c")
            binFile = binFile.substr(0, binFile.size() - 2);

        std::string cmd = "gcc \"" + outputFile + "\" -o \"" + binFile
                        + "\" -I/usr/local/include 2>&1";

        std::cout << "\033[1;34m[juliesp]\033[0m Compilation gcc...\n";
        int ret = system(cmd.c_str());

        if (ret != 0) {
            std::cerr << "\033[1;31m[Erreur]\033[0m Échec de la compilation gcc.\n";
            return 1;
        }
        std::cout << "\033[1;32m[juliesp]\033[0m Binaire généré : " << binFile << "\n";
    }

    delete root;
    return 0;
}
