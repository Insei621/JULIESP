//
// Created by kerya on 16/04/2026.
//

#include <complex>

#include "pch.h"
#include "Parser.h"

ASTNode* Parser::parseArithmetic() {
    Token opToken = showNext(); // On récupère le token actuel (+, -, numberq, etc.)
    int l = opToken.line;
    int c = opToken.cursor;
    TokenType type = opToken.type;

    // 1. Création du nœud parent
    SExpr* arithNode = new SExpr(l, c, false);

    // 2. On ajoute la Primitive en utilisant la valeur textuelle du token
    // (Supposons que ton Token a un champ 'value' ou 'text')
    arithNode->add(new Primitive(opToken.value, l, c, false));
    acceptIt(); // On avance après l'opérateur

    // 3. On parse les arguments
    int argCount = 0;
    while (showNext().type != TokenType::DEL_RBRACE) {
        arithNode->add(parseElement());
        argCount++;
    }

    // 4. Validation basée sur le TokenType
    validateAriArgs(type, argCount, l, c);

    expect(TokenType::DEL_RBRACE);
    return arithNode;
}

void Parser::validateAriArgs(TokenType type, int count, int l, int c) {
    switch (type) {
        case TokenType::CALC_PLUS:
        case TokenType::CALC_MULT:
            if (count < 2) throw std::logic_error("l"+std::to_string(l)+",c"+std::to_string(c)+" Operator requires at least 2 arguments.");
            break;

        case TokenType::CALC_NUMBERQ:
            if (count != 1) throw std::logic_error("l"+std::to_string(l)+",c"+std::to_string(c)+" numberq expects exactly 1 argument.");
            break;

        case TokenType::CALC_MOINS:
        case TokenType::CALC_DIV:
        case TokenType::CALC_EQ:
        case TokenType::CALC_INF:
        case TokenType::CALC_SUP:
        case TokenType::CALC_ADREQ:
            if (count != 2) throw std::logic_error("l"+std::to_string(l)+",c"+std::to_string(c)+"Operator expects exactly 2 arguments.");
            break;

        default:
            break;
    }
}