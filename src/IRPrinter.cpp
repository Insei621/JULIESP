//
// Created by kerya on 17/04/2026.
//

#include "../include/IRPrinter.h"


// -------------------- IRPrinter --------------------
void IRPrinter::printCFG(const ControlFlowGraph& cfg, std::ostream& out) {
    cfg.print(out);  // utilise la méthode existante
}

void IRPrinter::printModule(const IRModule& module, std::ostream& out) {
    module.print(out); // utilise la méthode existante
}

void IRPrinter::exportCFGtoDot(const ControlFlowGraph& cfg, const std::string& filename) {
    std::ofstream dotFile(filename);
    if (!dotFile) {
        std::cerr << "Erreur: impossible de créer " << filename << "\n";
        return;
    }

    dotFile << "digraph CFG {\n";
    dotFile << "  node [shape=record, fontname=\"Courier New\"];\n";

    for (const auto& bb : cfg.blocks) {
        dotFile << "  " << bb->label << " [label=\"{";
        dotFile << bb->label;
        for (const auto& inst : bb->instructions) {
            if (inst->op == IROp::LABEL) continue;
            std::string instStr = inst->toString();
            // Échappement des caractères spéciaux pour DOT
            std::string escaped;
            for (char c : instStr) {
                if (c == '"') escaped += "\\\"";
                else if (c == '\\') escaped += "\\\\";
                else if (c == '{') escaped += "\\{";
                else if (c == '}') escaped += "\\}";
                else if (c == '|') escaped += "\\|";
                else if (c == '<') escaped += "\\<";
                else if (c == '>') escaped += "\\>";
                else escaped += c;
            }
            dotFile << " | " << escaped;
        }
        dotFile << "}\"];\n";
        for (const auto& succ : bb->successors) {
            dotFile << "  " << bb->label << " -> " << succ->label << ";\n";
        }
    }
    dotFile << "}\n";
    dotFile.close();
    std::cout << "Fichier DOT généré : " << filename << "\n";
}

void IRPrinter::exportModuleToDot(const IRModule& module, const std::string& filename) {
    std::ofstream dotFile(filename);
    if (!dotFile) {
        std::cerr << "Erreur: impossible de créer " << filename << "\n";
        return;
    }

    dotFile << "digraph Module {\n";
    dotFile << "  compound=true;\n";
    dotFile << "  node [shape=record, fontname=\"Courier New\"];\n";

    // Fonction lambda pour écrire un CFG dans un cluster
    auto printCluster = [&](const ControlFlowGraph& cfg, const std::string& clusterName) {
        dotFile << "  subgraph cluster_" << clusterName << " {\n";
        dotFile << "    label=\"" << clusterName << "\";\n";
        for (const auto& bb : cfg.blocks) {
            dotFile << "    " << bb->label << " [label=\"{";
            dotFile << bb->label;
            for (const auto& inst : bb->instructions) {
                if (inst->op == IROp::LABEL) continue;
                std::string instStr = inst->toString();
                // Échappement simplifié (idem que ci-dessus)
                std::string escaped;
                for (char c : instStr) {
                    if (c == '"') escaped += "\\\"";
                    else if (c == '\\') escaped += "\\\\";
                    else if (c == '{') escaped += "\\{";
                    else if (c == '}') escaped += "\\}";
                    else if (c == '|') escaped += "\\|";
                    else escaped += c;
                }
                dotFile << " | " << escaped;
            }
            dotFile << "}\"];\n";
            for (const auto& succ : bb->successors) {
                dotFile << "    " << bb->label << " -> " << succ->label << ";\n";
            }
        }
        dotFile << "  }\n";
    };

    printCluster(module.mainCFG, "main");
    for (const auto& [name, cfg] : module.functions) {
        printCluster(cfg, name);
    }

    dotFile << "}\n";
    dotFile.close();
    std::cout << "Fichier DOT (module) généré : " << filename << "\n";
}

void IRPrinter::generatePNG(const std::string& dotFilename, const std::string& pngFilename) {
    std::string command = "dot -Tpng " + dotFilename + " -o " + pngFilename;
    int ret = system(command.c_str());
    if (ret != 0) {
        std::cerr << "Erreur: Graphviz dot non trouvé ou erreur de syntaxe.\n";
    } else {
        std::cout << "Image PNG générée : " << pngFilename << "\n";
    }
}