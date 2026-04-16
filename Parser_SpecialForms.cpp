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

ASTNode* Parser::parseSetq() {
    int l = showNext().line;
    int c = showNext().cursor;

    // 1. Création du nœud parent (S-Expression)
    SExpr* setqNode = new SExpr(l, c, false);

    // 2. Ajout du symbole 'setq' et consommation du token
    setqNode->add(new Primitive("setq", l, c, false));
    expect(TokenType::CORE_SETQ);

    // 3. Le premier argument DOIT être un identifiant (le nom de la variable)
    if (showNext().type == TokenType::IDENT) {
        setqNode->add(parseElement());
    } else {
        // Gestion d'erreur : setq attend un nom de variable
        throw std::logic_error("l"+std::to_string(showNext().line)+",c"+std::to_string(showNext().cursor)+
                         " Expected identifier after 'setq'");
    }

    // 4. Le deuxième argument est la valeur (une expression, un nombre, etc.)
    setqNode->add(parseElement());

    // 5. Fermeture de l'expression (on attend la parenthèse ou l'accolade droite)
    expect(TokenType::DEL_RBRACE);

    return setqNode;
}

ASTNode* Parser::parseLambda() {
    int l = showNext().line;
    int c = showNext().cursor;

    // 1. Création du nœud parent
    SExpr* lambdaNode = new SExpr(l, c, false);

    // 2. Ajout du symbole 'lambda'
    lambdaNode->add(new Primitive("lambda", l, c, false));
    expect(TokenType::CORE_LAMBDA);

    // 3. Parsage de la liste des PARAMÈTRES
    // On s'attend souvent à ce que les paramètres soient regroupés
    if (showNext().type == TokenType::DEL_LBRACE) {
        lambdaNode->add(parseElement()); // parseElement gérera la SExpr des arguments
    } else {
        // Optionnel : gérer le cas où un seul argument n'est pas entre parenthèses
        lambdaNode->add(parseElement());
    }

    // 4. Parsage du CORPS de la fonction
    // On attend une seule expression car en lisp il faut utiliser progn si on veux en exécuter plusieurs
    lambdaNode->add(parseElement());

    // 5. Fermeture de la S-Expression lambda
    expect(TokenType::DEL_RBRACE);

    return lambdaNode;
}

ASTNode* Parser::parseProgn() {
    int l = showNext().line;
    int c = showNext().cursor;

    // 1. Création du nœud parent
    SExpr* prognNode = new SExpr(l, c, false);

    // 2. Ajout du symbole 'progn' et consommation du token
    prognNode->add(new Primitive("progn", l, c, false));
    expect(TokenType::CORE_PROGN); // Ou le token correspondant à 'progn'

    // 3. On parse TOUTES les expressions présentes dans le bloc
    // Tant qu'on ne tombe pas sur la fermeture de la S-Expression
    while (showNext().type != TokenType::DEL_RBRACE) {
        prognNode->add(parseElement());
    }

    // 4. On ferme proprement
    expect(TokenType::DEL_RBRACE);

    return prognNode;
}

ASTNode* Parser::parseLoad() {
    int l = showNext().line;
    int c = showNext().cursor;

    // 1. Création du nœud parent
    SExpr* loadNode = new SExpr(l, c, false);

    // 2. Ajout du symbole 'load' et consommation du token
    loadNode->add(new Primitive("load", l, c, false));
    expect(TokenType::CORE_LOAD);

    // 3. Le paramètre doit être le chemin du fichier (souvent un String ou Identifier)
    // On parse l'élément qui représente le chemin
    if (showNext().type == TokenType::LIT_STRING || showNext().type == TokenType::IDENT) {
        loadNode->add(parseElement());
    } else {
        throw std::logic_error("l"+std::to_string(l)+",c"+std::to_string(c)+" The 'load' function expects a file path (string or identifier).");
    }

    // 4. Fermeture de l'expression
    expect(TokenType::DEL_RBRACE);

    return loadNode;
}

ASTNode* Parser::parsePrint() {
    int l = showNext().line;
    int c = showNext().cursor;

    // 1. Création du nœud parent
    SExpr* printNode = new SExpr(l, c, false);

    // 2. Ajout de la primitive 'print' et consommation du token
    printNode->add(new Primitive("print", l, c, false));
    expect(TokenType::CORE_PRINT);

    // 3. On parse tout ce qui suit jusqu'à la fermeture
    // Cela permet de faire : {print "Valeur de x : " x}
    while (showNext().type != TokenType::DEL_RBRACE) {
        printNode->add(parseElement());
    }

    // 4. Fermeture de l'expression
    expect(TokenType::DEL_RBRACE);

    return printNode;
}