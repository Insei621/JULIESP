//
// Created by kerya on 17/04/2026.
//

#ifndef COMPILATEUR_JULIESP_IRPRINTER_H
#define COMPILATEUR_JULIESP_IRPRINTER_H
#include "pch.h"
#include "IRGenerator.h"

class IRPrinter {
public:
    // Affichage console d'un CFG
    static void printCFG(const ControlFlowGraph& cfg, std::ostream& out = std::cout);
    // Affichage console d'un module entier
    static void printModule(const IRModule& module, std::ostream& out = std::cout);

    // Export DOT d'un CFG (fichier .dot)
    static void exportCFGtoDot(const ControlFlowGraph& cfg, const std::string& filename);
    // Export DOT d'un module (avec sous-graphes pour chaque fonction)
    static void exportModuleToDot(const IRModule& module, const std::string& filename);

    // Génération PNG via Graphviz (appelle dot)
    static void generatePNG(const std::string& dotFilename, const std::string& pngFilename);
};


#endif //COMPILATEUR_JULIESP_IRPRINTER_H