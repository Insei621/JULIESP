//
// Created by kerya on 10/05/2026.
//

#ifndef JULIESP_COMPILERCONFIG_H
#define JULIESP_COMPILERCONFIG_H

#include "pch.h"

struct CompilerConfig {
    std::string sourceFile;
    std::string outputFile  = "output/output.c";
    std::string binaryName  = "";
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

    // Retourne false si le programme doit s'arrêter (--help, erreur)
    static bool parse(int argc, char** argv, CompilerConfig& cfg);
    static void printHelp(const char* progName);
};


#endif //JULIESP_COMPILERCONFIG_H