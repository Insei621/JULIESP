//
// Created by kerya on 08/04/2026.
//

#ifndef COMPILATEUR_JULIESP_PARSER_H
#define COMPILATEUR_JULIESP_PARSER_H

#include "pch.h"
#include "AST.h"

class Parser {
public:
    // Constructeur : on passe le vecteur de tokens issus du Lexer
    explicit Parser(std::vector<Token> t);

    // --- Les 3 méthodes demandées ---

    /** @brief Retourne le lexème courant sans avancer */
    Token showNext() const;

    /** @brief Consomme le lexème courant et avance à l'index suivant */
    void acceptIt();

    /** @brief Vérifie le type du lexème courant, l'accepte si valide, sinon erreur */
    void expect(TokenType type);

    // --- Point d'entrée principal du parsing ---
    ASTNode* parse();

    friend int main(int argc, char** argv);      // pour les test j'ai besoin d'accéder au fonction du parser dansa le main
    std::vector<ASTNode*> parseProgram();

private:
    std::vector<Token> tokens;
    size_t current = 0;

    // Méthodes de parsing récursif
    ASTNode* parseElement(bool quoted = false);
    ASTNode* parseSExpr(bool quoted = false);
    ASTNode* parseAtom(bool quoted = false);
    ASTNode* parseDefaultList(bool quoted = false);

    // Formes spéciales (dans Parser_SpecialForms.cpp)
    ASTNode* parseIf();
    ASTNode* parseSetq();
    ASTNode* parseLambda();
    ASTNode* parseProgn();
    ASTNode* parseLoad();
    ASTNode* parsePrint();
    ASTNode* parseScan();

    // Primitives (dans Parser_Primitives.cpp)
    void validateAriArgs(TokenType type, int count, int l, int c);
    ASTNode* parseArithmetic();
    void validatePrimitiveArgs(TokenType type, int count, int l, int c, std::string name);
    ASTNode* parsePrimitiveCall();

    /** @brief Vérifie si on a parcouru tous les tokens */
    bool isAtEnd() const;

    // Utilitaires pour le load
    std::string readFile(const std::string& path);

    std::string getExecutablePath();

};




#endif //COMPILATEUR_JULIESP_PARSER_H