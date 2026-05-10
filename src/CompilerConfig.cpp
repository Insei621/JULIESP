//
// Created by kerya on 10/05/2026.
//

#include "CompilerConfig.h"

#include "../include/CompilerConfig.h"
#include "../include/DebugPrinter.h"
#include "pch.h"
#include <iostream>


void CompilerConfig::printHelp(const char* progName) {
    std::cout << "\n";
    std::cout << "  juliesp — Compilateur du langage JULIESP\n";
    std::cout << "\n";
    std::cout << "Usage:\n";
    std::cout << "  " << progName << " <fichier.jlsp> [options]\n";
    std::cout << "\n";
    std::cout << "Options de sortie:\n";
    std::cout << "  -o, --output <fichier.c>   Fichier de sortie C (défaut: output/output.c)\n";
    std::cout << "  -c, --compile              Compile le C généré avec gcc\n";
    std::cout << "  -b, --binary <nom>         Nom du binaire (défaut: nom du fichier C sans .c)\n";
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
    std::cout << "  " << progName << " programme.jlsp -d\n";
    std::cout << "\n";
}

bool CompilerConfig::parse(int argc, char** argv, CompilerConfig& cfg) {
    if (argc < 2) {
        std::cerr << "Usage: juliesp <fichier.jlsp> [options]\n";
        std::cerr << "Utilisez -h pour l'aide.\n";
        return false;
    }

    if (std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0) {
        printHelp(argv[0]);
        return false;
    }

    cfg.sourceFile = argv[1];

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-o" || arg == "--output") {
            if (i + 1 >= argc) { DebugPrinter::logError(arg + " attend un nom de fichier."); return false; }
            cfg.outputFile = argv[++i];
        }
        else if (arg == "-b" || arg == "--binary") {
            if (i + 1 >= argc) { DebugPrinter::logError(arg + " attend un nom de binaire."); return false; }
            cfg.binaryName = argv[++i];
        }
        else if (arg == "-c" || arg == "--compile")      cfg.compileOutput = true;
        else if (arg == "-r" || arg == "--run")        { cfg.runOutput = true; cfg.compileOutput = true; }
        else if (arg == "-d" || arg == "--debug")        cfg.dumpLex = cfg.dumpAst = cfg.dumpImgAst = cfg.dumpIR = true;
        else if (arg == "-dl" || arg == "--dump-lex")    cfg.dumpLex     = true;
        else if (arg == "-da" || arg == "--dump-ast")    cfg.dumpAst     = true;
        else if (arg == "-di" || arg == "--dump-imgast") cfg.dumpImgAst  = true;
        else if (arg == "-dr" || arg == "--dump-ir")     cfg.dumpIR      = true;
        else if (arg == "--lex-only")                    cfg.lexOnly     = true;
        else if (arg == "--parse-only")                  cfg.parseOnly   = true;
        else if (arg == "--sem-only")                    cfg.semOnly     = true;
        else if (arg == "--ir-only")                     cfg.irOnly      = true;
        else {
            DebugPrinter::logError("Option inconnue : " + arg);
            std::cerr << "Utilisez -h pour voir les options disponibles.\n";
            return false;
        }
    }
    return true;
}