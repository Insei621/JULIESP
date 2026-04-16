//
// Created by kerya on 16/04/2026.
//

#include "pch.h"
#include "Parser.h"

ASTNode* Parser::parseIf() {
    int l = showNext().line;
    int c = showNext().cursor;

    // 1. Création du nœud parent
    SExpr* ifNode = new SExpr(l, c, false);

    // 2. On ajoute le symbole 'if' et on avance
    ifNode->add(new Primitive("if", l, c, false));
    expect(TokenType::CORE_IF);

    // 3. La CONDITION est obligatoire
    ifNode->add(parseElement());

    // 4. Le bloc THEN est obligatoire
    ifNode->add(parseElement());

    // 5. Le bloc ELSE est facultatif
    if (showNext().type != TokenType::DEL_RBRACE) {
        // S'il reste quelque chose avant la fermeture, c'est le else
        ifNode->add(parseElement());
    } else {
        // on ajoute un nœud "Null" pour laisser la liste à 3 éléments
        ifNode->add(new Identifier("nil", l, c, false));
    }

    // 6. On ferme la S-Expression
    expect(TokenType::DEL_RBRACE);

    return ifNode;
}
