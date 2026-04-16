
#include "pch.h"
#include "Lexer.h"
#include "Parser.h"
#include "PrettyPrinter.h"
#include "tests_bench/Lexer_tb.hpp"

#include <iostream>
#include <cstring> // strcmp



int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " commande\n";
        return 1;
    }

    if (std::strcmp(argv[1], "LEX_TEST") == 0) {
        run_test();
        return 0;
    }

    /// Analyse lexicale
    Lexer lexer(argv[1]);   //on passe le code source en paramètre
    std::vector<Token> tokens = lexer.tokenize();

    /// Analyse syntaxique et construction de l'AST
    Parser parser(tokens);
    ASTNode* root = parser.parse();

    /// Impression de l'AST avec le PrettyPrinter
    PrettyPrinter printer;
    root->accept(&printer);
    std::cout << std::endl;



}
/*
#include <iostream>
#include <vector>
#include "Parser.h"
#include "AST.h"

    int main() {
        // On simule : {if {> x 5} "OK" "NON"}
        // Cela permet de tester :
        // 1. Le IF
        // 2. Une S-Expression imbriquée (le calcul >)
        // 3. Des Atomes de types différents (Identifiant, Entier, String)

        std::vector<Token> mockTokens = {
            {TokenType::DEL_LBRACE, "{", 1, 0},
            {TokenType::CORE_IF,    "if", 1, 1},
                {TokenType::DEL_LBRACE, "{", 1, 4},
                {TokenType::CALC_SUP,   ">", 1, 5},
                {TokenType::IDENT,      "x", 1, 7},
                {TokenType::LIT_INT,    "5", 1, 9},
                {TokenType::DEL_RBRACE, "}", 1, 10},
            {TokenType::LIT_STRING, "OK", 1, 12},
            {TokenType::LIT_STRING, "NON", 1, 17},
            {TokenType::DEL_RBRACE, "}", 1, 21}
        };

        try {
            Parser parser(mockTokens);

            // On lance le parsing à partir de parseElement
            ASTNode* root = parser.parseElement();

            if (root) {
                std::cout << "--- AST Arbre de Syntaxe Abstraite ---" << std::endl;
                root->print(0); // 0 pour l'indentation de départ
                std::cout << "--------------------------------------" << std::endl;

                delete root; // N'oublie pas de nettoyer la mémoire !
            }
        } catch (const std::exception& e) {
            std::cerr << "Erreur lors du test : " << e.what() << std::endl;
        }

        return 0;
    }

/// Analyse syntaxique
//    Parser parser(tokens);



}
*/