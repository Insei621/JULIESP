//
// Created by keryan on 08/04/2026.
//
#ifndef COMPILATEUR_JULIESP_LEXER_H
#define COMPILATEUR_JULIESP_LEXER_H

#include "pch.h"

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <regex>
#include "pch.h"

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> tokenize();

private:
    struct Rule {
        std::regex pattern;
        TokenType type;
        bool skip;
    };
    static const std::vector<Rule> rules;

    std::string source;
    std::string_view view;
    size_t cursor = 0;
    int line = 1;

    void advance(std::string_view matched);

    void reporting_erreurs(const std::vector<Token>& tokens) const;
};


#endif //COMPILATEUR_JULIESP_LEXER_H