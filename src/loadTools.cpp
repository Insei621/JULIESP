//
// Created by kerya on 28/04/2026.
//

#include "loadTools.h"

std::string loadTools::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Impossible d'ouvrir le fichier : " + path);
    }
    return std::string((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
}

std::vector<ASTNode*> loadTools::parseProgram() {
    std::vector<ASTNode*> nodes;
    while (showNext().type != TokenType::END_OF_FILE) {
        nodes.push_back(parseElement());
    }
    return nodes;
}