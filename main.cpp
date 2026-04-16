#include "pch.h"
#include "Lexer.h"
#include "tests_bench/Lexer_tb.hpp"

#include <iostream>
#include <cstring> // strcmp

#include "Parser.h"


int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " commande\n";
        return 1;
    }

    if (std::strcmp(argv[1], "LEX_TEST") == 0) {
        run_lexer_test();
        return 0;
    }
/// Analyse lexicale
    Lexer lexer(argv[1]);
    std::vector<Token> tokens = lexer.tokenize();

/// Analyse syntaxique
//    Parser parser(tokens);



}
