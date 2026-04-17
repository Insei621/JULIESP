#ifndef LEXER_TB_HPP
#define LEXER_TB_HPP

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include "../Lexer.h"
#include "../pch.h"

/**
 * Affichage lisible des TokenType
 */
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {

        // --- Identifiants / Littéraux ---
        case TokenType::IDENT:        return "IDENT";
        case TokenType::LIT_INT:      return "LIT_INT";
        case TokenType::LIT_FLOAT:    return "LIT_FLOAT";

        // --- Booléens ---
        case TokenType::BOOL_TRUE:    return "BOOL_TRUE";
        case TokenType::BOOL_FALSE:   return "BOOL_FALSE";

        // --- Espaces / commentaires ---
        case TokenType::DEL_SPACE:    return "DEL_SPACE";
        case TokenType::COM_LINE:     return "COM_LINE";
        case TokenType::COM_BLOCK:    return "COM_BLOCK";

        // --- Délimiteurs ---
        case TokenType::DEL_LBRACE:   return "DEL_LBRACE";
        case TokenType::DEL_RBRACE:   return "DEL_RBRACE";

        // --- Noyau Lisp ---
        case TokenType::CORE_IF:      return "CORE_IF";
        case TokenType::CORE_SETQ:    return "CORE_SETQ";
        case TokenType::CORE_PROGN:   return "CORE_PROGN";
        case TokenType::CORE_LOAD:    return "CORE_LOAD";
        case TokenType::CORE_LAMBDA:  return "CORE_LAMBDA";
        case TokenType::CORE_QUOTE:   return "CORE_QUOTE";

        // --- Calcul ---
        case TokenType::CALC_PLUS:    return "CALC_PLUS";
        case TokenType::CALC_MOINS:   return "CALC_MOINS";
        case TokenType::CALC_MULT:    return "CALC_MULT";
        case TokenType::CALC_DIV:     return "CALC_DIV";
        case TokenType::CALC_INF:     return "CALC_INF";
        case TokenType::CALC_SUP:     return "CALC_SUP";
        case TokenType::CALC_EQ:      return "CALC_EQ";
        case TokenType::CALC_ADREQ:   return "CALC_ADREQ";
        case TokenType::CALC_NUMBERQ:return "CALC_NUMBERQ";

        // --- Listes ---
        case TokenType::MAIN_CAR:     return "MAIN_CAR";
        case TokenType::MAIN_CDR:     return "MAIN_CDR";
        case TokenType::MAIN_CONS:    return "MAIN_CONS";
        case TokenType::MAIN_NULL:    return "MAIN_NULL";
        case TokenType::MAIN_ATOM:    return "MAIN_ATOM";

            // --- À ajouter dans ton switch ---
        case TokenType::LIT_STRING:   return "LIT_STRING";
        case TokenType::LIT_CHAR:     return "LIT_CHAR";
        case TokenType::CORE_PRINT:   return "CORE_PRINT"; // Pour le symbole €

        // --- Fin / erreur ---
        case TokenType::END_OF_FILE:  return "EOF";
        case TokenType::UNKNOWN:      return "UNKNOWN";

        default:                      return "???";
    }
}

/**
 * Fonction de test principale
 */
inline void run_test() {
    std::string testCode = R"(
§!
   Ce bloc test les commentaires multi-lignes
   ainsi que les structures très profondes.
!§

( : complexe ( & ( + 1 2 ) ( & ( ? µ 42 0 ) ²( a b ( c d ) ) ) ) )

§§ Test des symboles isolés dans une liste
( € ( @ ( << complexe ) ) )

( : x 0.0001 ) §§ Test des petits décimaux
        )";

    std::cout << "=== Code test ===\n";
    std::cout << testCode << std::endl;

    std::cout << "=== Test du Lexer JulieSP ===\n";

    Lexer lexer(testCode);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << std::left
              << std::setw(15) << "TYPE"
              << std::setw(15) << "VALUE"
              << std::setw(8)  << "LINE"
              << "COL\n";

    std::cout << std::string(50, '-') << '\n';

    for (const auto& t : tokens) {
        std::cout << std::left
                  << std::setw(15) << tokenTypeToString(t.type)
                  << std::setw(15) << t.value
                  << std::setw(8)  << t.line
                  << t.cursor << '\n';
    }

    std::cout << " === Test du Parseur ===" <<std::endl<<std::flush;
    Parser parser(tokens);
    ASTNode* root = parser.parse();
    PrettyPrinter printer;
    root->accept(&printer);
    std::cout << std::endl;

}

#endif