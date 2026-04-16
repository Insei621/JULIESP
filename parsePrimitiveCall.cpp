//
// Created by kerya on 16/04/2026.
//
#include "pch.h"
#include "Parser.h"

ASTNode* Parser::parsePrimitiveCall() {
    Token opToken = showNext();
    int l = opToken.line;
    int c = opToken.cursor;
    TokenType type = opToken.type;

    SExpr* primNode = new SExpr(l, c, false);

    // 1. Ajout de la primitive (car, cdr, cons, etc.)
    primNode->add(new Primitive(opToken.value, l, c, false));
    acceptIt(); // Consomme le token de la primitive

    // 2. Parsage des arguments
    int argCount = 0;
    while (showNext().type != TokenType::DEL_RBRACE) {
        primNode->add(parseElement());
        argCount++;
    }

    // 3. Validation du nombre d'arguments
    validatePrimitiveArgs(type, argCount, l, c, opToken.value);

    expect(TokenType::DEL_RBRACE);
    return primNode;
}

void Parser::validatePrimitiveArgs(TokenType type, int count, int l, int c, std::string name) {
    switch (type) {
        // Primitives UNAIRES (1 seul argument)
        case TokenType::MAIN_CAR:
        case TokenType::MAIN_CDR:
        case TokenType::MAIN_NULL:
        case TokenType::MAIN_ATOM:
            if (count != 1) {
                throw std::logic_error("l"+std::to_string(l)+",c"+std::to_string(c)+",name: "+name+" expects exactly 1 argument.");
            }
            break;

            // Primitives BINAIRES (2 arguments)
        case TokenType::MAIN_CONS:
            if (count != 2) {
                throw std::logic_error("l"+std::to_string(l)+",c"+std::to_string(c)+",name: "+name+" expects exactly 2 arguments.");
            }
            break;

        default:
            // Si on arrive ici, c'est que le type n'est pas géré
            break;
    }
}