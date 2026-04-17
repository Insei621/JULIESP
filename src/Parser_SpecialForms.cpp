//
// Created by kerya on 16/04/2026.
//

#include "../include/pch.h"
#include "../include/Parser.h"

ASTNode* Parser::parseIf() {
    int l = showNext().line;
    int c = showNext().cursor;

    // On ne consomme pas le '?' ici si parseSExpr l'a déjà fait ?
    // Attention : si ton switch dans parseSExpr n'a pas fait acceptIt(),
    // alors ton expect(TokenType::CORE_IF) est correct.
    expect(TokenType::CORE_IF);

    SExpr* ifNode = new SExpr(l, c, false);
    ifNode->add(new Primitive("?", l, c, false));

    ifNode->add(parseElement()); // Condition
    ifNode->add(parseElement()); // Then

    if (showNext().type != TokenType::DEL_RBRACE) {
        ifNode->add(parseElement()); // Else
    } else {
        ifNode->add(new Identifier("nil", l, c, false));
    }

    // --- LA CORRECTION EST ICI ---
    expect(TokenType::DEL_RBRACE); // On ferme le bloc IF !
    // -----------------------------

    return ifNode;
}

ASTNode* Parser::parseSetq() {
    int l = showNext().line;
    int c = showNext().cursor;

    expect(TokenType::CORE_SETQ);

    // 1. Création du nœud parent (S-Expression)
    SExpr* setqNode = new SExpr(l, c, false);

    // 2. Ajout du symbole 'setq' et consommation du token
    setqNode->add(new Primitive("setq", l, c, false));

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
    // 5. On ferme la parenthèse
    expect(TokenType::DEL_RBRACE);

    return setqNode;
}

ASTNode* Parser::parseLambda() {
    int l = showNext().line;
    int c = showNext().cursor;

    expect(TokenType::CORE_LAMBDA);

    // 1. Création du nœud parent
    SExpr* lambdaNode = new SExpr(l, c, false);

    // 2. Ajout du symbole 'lambda'
    lambdaNode->add(new Primitive("lambda", l, c, false));

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

    // 5. On ferme la parenthèse
    expect(TokenType::DEL_RBRACE);

    return lambdaNode;
}

ASTNode* Parser::parseProgn() {
    int l = showNext().line;
    int c = showNext().cursor;

    expect(TokenType::CORE_PROGN);

    // 1. Création du nœud parent
    SExpr* prognNode = new SExpr(l, c, false);

    // 2. Ajout du symbole 'progn' et consommation du token
    prognNode->add(new Primitive("progn", l, c, false));

    // 3. On parse TOUTES les expressions présentes dans le bloc
    // Tant qu'on ne tombe pas sur la fermeture de la S-Expression
    while (showNext().type != TokenType::DEL_RBRACE) {
        prognNode->add(parseElement());
    }
    // 5. On ferme la parenthèse
    expect(TokenType::DEL_RBRACE);

    return prognNode;


}

ASTNode* Parser::parseLoad() {
    int l = showNext().line;
    int c = showNext().cursor;

    expect(TokenType::CORE_LOAD);

    // 1. Création du nœud parent
    SExpr* loadNode = new SExpr(l, c, false);

    // 2. Ajout du symbole 'load' et consommation du token
    loadNode->add(new Primitive("load", l, c, false));

    // 3. Le paramètre doit être le chemin du fichier (souvent un String ou Identifier)
    // On parse l'élément qui représente le chemin
    if (showNext().type == TokenType::LIT_STRING || showNext().type == TokenType::IDENT) {
        loadNode->add(parseElement());
    } else {
        throw std::logic_error("l"+std::to_string(l)+",c"+std::to_string(c)+" The 'load' function expects a file path (string or identifier).");
    }

    // 5. On ferme la parenthèse
    expect(TokenType::DEL_RBRACE);

    return loadNode;
}

ASTNode* Parser::parsePrint() {
    int l = showNext().line;
    int c = showNext().cursor;

    expect(TokenType::CORE_PRINT);

    // 1. Création du nœud parent
    SExpr* printNode = new SExpr(l, c, false);

    // 2. Ajout de la primitive 'print' et consommation du token
    printNode->add(new Primitive("print", l, c, false));

    // 3. On parse tout ce qui suit jusqu'à la fermeture
    // Cela permet de faire : {print "Valeur de x : " x}
    while (showNext().type != TokenType::DEL_RBRACE) {
        printNode->add(parseElement());
    }

    // 5. On ferme la parenthèse
    expect(TokenType::DEL_RBRACE);

    return printNode;
}

ASTNode* Parser::parseScan() {
    int l = showNext().line;
    int c = showNext().cursor;

    // 1. Consomme le mot-clé 'scan'
    expect(TokenType::CORE_SCAN);

    // 2. On parse exactement UN élément
    ASTNode* arg = parseElement();

    // 3. Vérification : l'argument est-il un littéral ?
    // On vérifie si le nœud n'est ni une SExpr, ni un Identifier.
    // (Ou on vérifie explicitement s'il appartient à tes classes de littéraux)
    if (dynamic_cast<SExpr*>(arg) || dynamic_cast<Identifier*>(arg)) {
        throw std::runtime_error("Erreur ligne " + std::to_string(l) +
                                 " : 'scan' attend un littéral (String, Int, ou Float) en argument.");
    }

    // 4. On s'assure qu'il n'y a pas d'autres arguments avant la fermeture
    // Si le token suivant n'est pas '}', c'est qu'il y a trop d'arguments.
    if (showNext().type != TokenType::DEL_RBRACE) {
        throw std::runtime_error("Erreur ligne " + std::to_string(l) +
                                  " : 'scan' ne peut prendre qu'un seul argument.");
    }

    // 5. Construction du nœud AST
    SExpr* scanNode = new SExpr(l, c, false);
    scanNode->add(new Primitive("scan", l, c, false));
    scanNode->add(arg);

    // 6. Consomme la parenthèse/accolade fermante
    expect(TokenType::DEL_RBRACE);

    return scanNode;
}