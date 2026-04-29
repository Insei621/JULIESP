//
// Created by kerya on 16/04/2026.
//

#include "../include/pch.h"
#include "../include/Parser.h"
#include "../include/Lexer.h"

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

    Token pathTok = showNext();          // ← capture avant
    expect(TokenType::LIT_STRING);       // ← puis consomme
    expect(TokenType::DEL_RBRACE);

    // Lire le fichier
    std::string content = readFile(pathTok.value);

    // Lexer + Parser sur le contenu du fichier
    Lexer subLexer(content);
    std::vector<Token> subTokens = subLexer.tokenize();
    Parser subParser(subTokens);
    std::vector<ASTNode*> externalNodes = subParser.parseProgram();

    // Crée un nœud progn qui contient tout le fichier chargé
    SExpr* loadedContent = new SExpr(l, c, false);
    loadedContent->add(new Primitive("progn", l, c, false));
    for (ASTNode* node : externalNodes) {
        loadedContent->add(node);
    }

    return loadedContent;
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

    // 1. Consomme le mot-clé 'ç' (CORE_SCAN)
    expect(TokenType::CORE_SCAN);

    // 2. Création immédiate du nœud parent SExpr
    SExpr* scanNode = new SExpr(l, c, false);

    // 3. Ajout de la primitive 'ç' (pour que l'IRGenerator le reconnaisse)
    // On utilise "ç" car c'est ce que ton Lexer a capturé et ce que ton IR attend
    scanNode->add(new Primitive("ç", l, c, false));

    // 4. On parse l'argument (la variable qui va recevoir la saisie)
    ASTNode* arg = parseElement();

    // 5. VÉRIFICATION : L'argument DOIT être une variable (Identifier)
    // On ne peut pas scanner une valeur dans un nombre fixe comme (ç 10)
    if (!dynamic_cast<Identifier*>(arg)) {
        throw std::runtime_error("Erreur ligne " + std::to_string(l) +
                                 " : 'ç' attend un nom de variable (Identifier) en argument.");
    }

    scanNode->add(arg);

    // 6. Vérification du nombre d'arguments
    if (showNext().type != TokenType::DEL_RBRACE) {
        throw std::runtime_error("Erreur ligne " + std::to_string(l) +
                                  " : 'ç' ne peut prendre qu'un seul argument.");
    }

    // 7. Consomme la parenthèse fermante
    expect(TokenType::DEL_RBRACE);

    return scanNode;
}