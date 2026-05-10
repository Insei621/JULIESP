//
// main.cpp — Point d'entrée du compilateur JULIESP
//

#include "../include/pch.h"
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/SemanticAnalyzer.h"
#include "../include/IRGenerator.h"
#include "../include/CGenerator.h"
#include "../include/DebugPrinter.h"

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
    std::cout << "  juliesp — Compilateur du langage JULIESP\n";
    std::cout << "\n";
    std::cout << "Usage:\n";
    std::cout << "  " << progName << " <fichier.jlsp> [options]\n";
    std::cout << "\n";
    std::cout << "Options de sortie:\n";
    std::cout << "  -o, --output <fichier.c>   Fichier de sortie C (défaut: output/output.c)\n";
    std::cout << "  -c, --compile              Compile le C généré avec gcc\n";
    std::cout << "  -b, --binary <nom>         Nom du binaire exécutable (défaut: même nom que -o sans .c)\n";
    std::cout << "  -r, --run                  Exécute le binaire après compilation (implique -c)\n";
    std::cout << "\n";
    std::cout << "Options de debug:\n";
    std::cout << "  -d,  --debug               Active tous les dumps (-dl -da -di -dr)\n";
    std::cout << "  -dl, --dump-lex            Affiche les tokens\n";
    std::cout << "  -da, --dump-ast            Affiche l'AST dans le terminal\n";
    std::cout << "  -di, --dump-imgast         Génère ast.png dans le dossier de sortie\n";
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
    std::cout << "  " << progName << " programme.jlsp -c\n";
    std::cout << "  " << progName << " programme.jlsp -c -b mon_programme\n";
    std::cout << "  " << progName << " programme.jlsp -r\n";
    std::cout << "  " << progName << " programme.jlsp -o mon_prog.c -b mon_prog -r\n";
    std::cout << "  " << progName << " programme.jlsp -d\n";
    std::cout << "  " << progName << " programme.jlsp -da -dr\n";
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
    std::string binaryName  = "";  // vide = dérivé du outputFile
    bool dumpLex       = false;
    bool dumpAst       = false;
    bool dumpImgAst    = false;
    bool dumpIR        = false;
    bool lexOnly       = false;
    bool parseOnly     = false;
    bool semOnly       = false;
    bool irOnly        = false;
    bool compileOutput = false;
    bool runOutput     = false;

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
        else if (arg == "-b" || arg == "--binary") {
            if (i + 1 >= argc) {
                std::cerr << "\033[1;31m[Erreur]\033[0m "
                          << arg << " attend un nom de binaire.\n";
                return 2;
            }
            binaryName = argv[++i];
        }
        else if (arg == "-c" || arg == "--compile")      compileOutput = true;
        else if (arg == "-r" || arg == "--run") {
            runOutput     = true;
            compileOutput = true;  // -r implique -c
        }

        // Debug
        else if (arg == "-d"  || arg == "--debug") {
            dumpLex = dumpAst = dumpImgAst = dumpIR = true;
        }
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

    std::cout << "\033[1;34m[Lexage]\033[0m Lancement de l'analyse...\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    std::cout << "\033[1;32m[Succès]\033[0m Analyse lexicale terminée.\n";

    if (dumpLex) DebugPrinter::dumpLex(lexer, tokens);

    if (lexOnly) return 0;

    // ==========================================================================
    // ÉTAPE 3 : Analyse syntaxique
    // ==========================================================================

    std::string sourceDir = std::filesystem::path(sourceFile)
                            .parent_path()
                            .string();

    std::cout << "\033[1;34m[Parsing]\033[0m Lancement de l'analyse...\n";
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

    if (dumpAst)    DebugPrinter::dumpAst(root);
    if (dumpImgAst) DebugPrinter::dumpImgAst(root, outputFile);

    if (parseOnly) return 0;

    // ==========================================================================
    // ÉTAPE 4 : Analyse sémantique
    // ==========================================================================

    std::cout << "\033[1;34m[Sémantique]\033[0m Lancement de l'analyse...\n";
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

    std::cout << "\033[1;34m[IR]\033[0m Génération de la représentation intermédiaire...\n";
    IRGenerator irGen;
    IRProgram ir = irGen.generate(root);
    std::cout << "\033[1;32m[Succès]\033[0m IR générée.\n";

    if (dumpIR) DebugPrinter::dumpIR(irGen, ir);

    if (irOnly) return 0;

    // ==========================================================================
    // ÉTAPE 6 : Génération C
    // ==========================================================================

    std::cout << "\033[1;34m[CGen]\033[0m Génération du code C...\n";
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
        // Détermine le nom du binaire
        std::string binFile;
        if (!binaryName.empty()) {
            // -b fourni explicitement
            binFile = binaryName;
        } else {
            // Dérive du outputFile : retire .c
            binFile = outputFile;
            if (binFile.size() > 2 && binFile.substr(binFile.size() - 2) == ".c")
                binFile = binFile.substr(0, binFile.size() - 2);
        }

        std::string cmd = "gcc \"" + outputFile + "\" -o \"" + binFile
                        + "\" -I/usr/local/include 2>&1";

        std::cout << "\033[1;34m[juliesp]\033[0m Compilation gcc...\n";
        int ret = system(cmd.c_str());

        if (ret != 0) {
            std::cerr << "\033[1;31m[Erreur]\033[0m Échec de la compilation gcc.\n";
            return 1;
        }
        std::cout << "\033[1;32m[juliesp]\033[0m Binaire généré : " << binFile << "\n";

        // ==========================================================================
        // ÉTAPE 8 (optionnelle) : Exécution du binaire
        // ==========================================================================

        if (runOutput) {
            std::cout << "\033[1;34m[juliesp]\033[0m Exécution de : " << binFile << "\n";
            std::cout << std::string(40, '-') << "\n";
            int runRet = system(("\"" + binFile + "\"").c_str());
            std::cout << std::string(40, '-') << "\n";
            if (runRet != 0)
                std::cerr << "\033[1;33m[Attention]\033[0m Le programme s'est terminé avec le code " 
                          << runRet << "\n";
        }
    }

    delete root;
    return 0;
}
