//
// Created by kerya on 08/04/2026.
//
#include <iostream>
#include <vector>
#include <regex>
#include <string>
#include <cctype>
#include <string_view>
#include <utility>
#include <fstream>
#include <stdexcept>
#include <iomanip>
#include <utility>
#include <stdexcept>
#include <ostream>
#include <sstream>
#include <variant>
#include <filesystem>
#include <cstdlib> // Pour system()
#include <unordered_map>
#include <fstream>
using namespace std;

#ifndef COMPILATEUR_JULIESP_PCH_H
#define COMPILATEUR_JULIESP_PCH_H

#define CLR_RESET  "\033[0m"
#define CLR_BOLD   "\033[1m"
#define CLR_RED    "\033[1;31m"
#define CLR_GREEN  "\033[1;32m"
#define CLR_BLUE   "\033[1;34m"
#define CLR_YELLOW "\033[1;33m"
#define CLR_CYAN   "\033[1;36m"

enum class TokenType {
    IDENT, LIT_INT, LIT_FLOAT, LIT_STRING, LIT_CHAR, DEL_SPACE,
    DEL_RBRACE, DEL_LBRACE, COM_LINE, COM_BLOCK, CORE_QUOTE,
    CORE_IF, CORE_SETQ, CORE_LAMBDA, CORE_PROGN, CORE_LOAD, CORE_SCAN,
    CORE_PRINT, MAIN_CAR, MAIN_CDR, MAIN_CONS, MAIN_NULL, MAIN_ATOM,
    CALC_PLUS, CALC_MOINS, CALC_MULT, CALC_DIV, CALC_INF,
    CALC_SUP, CALC_EQ, CALC_ADREQ, CALC_NUMBERQ, BOOL_TRUE,
    BOOL_FALSE, END_OF_FILE, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line; int cursor;
};

#endif //COMPILATEUR_JULIESP_PCH_H