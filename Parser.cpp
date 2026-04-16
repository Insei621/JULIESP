//
// Created by keryan on 08/04/2026.
//

#include "Parser.h"
#include "pch.h"

Parser::Parser(std::vector<Token> t) : tokens(std::move(t)), current(0) {}

ASTNode* Parser::parse() {
    // 1. On récupère les coordonnées du début du fichier
    int l = showNext().line;
    int c = showNext().cursor;

    // 2. Si ton langage supporte plusieurs expressions à la suite (top-level),
    // on les regroupe souvent dans un nœud racine.
    // Si tu ne veux parser qu'une seule expression, retire la boucle.

    // Cas courant : on parse une seule expression complète
    ASTNode* root = parseElement();

    // 3. Optionnel : vérifier qu'on est bien à la fin du fichier (EOF)
    // Cela évite d'ignorer du code mal formé à la fin
    if (showNext().type != TokenType::END_OF_FILE) {
        // Tu peux choisir de lever une erreur ou de boucler pour
        // gérer plusieurs expressions.
    }

    return root;
}

/// --- 1.Logique de parsing
// 1. Retourne le lexème courant sans avancer
Token Parser::showNext() const {
    if (isAtEnd()) {
        return Token(TokenType::END_OF_FILE,"-1", -1, -1);
    }
    return tokens[current];
}

// 2. Avance simplement à l'index suivant
void Parser::acceptIt() {
    if (!isAtEnd()) {
        current++;
    }
}

// 3. LA VÉRIFICATION : Le type du Token correspond-il à l'un des types attendus
void Parser::expect(TokenType expectedType) {
    Token actualToken = showNext();

    if (actualToken.type == expectedType) {
        // Le type est valide ! On consomme le token pour passer au suivant.
        acceptIt();
    } else {
        // Le type est invalide : on lève une erreur détaillée
        std::string message = "Erreur de syntaxe à la ligne " + std::to_string(actualToken.line) +
                              " : attendu type " + std::to_string(static_cast<int>(expectedType)) +
                              ", mais reçu " + std::to_string(static_cast<int>(actualToken.type));

        throw std::runtime_error(message);
    }
}

bool Parser::isAtEnd() const {
    return current >= tokens.size();
}

/// --- 2.Parsing
ASTNode* Parser::parseSExpr(bool quoted) {
    expect(TokenType::DEL_LBRACE);
    SExpr* node = new SExpr(showNext().line, showNext().cursor, quoted);

    while (showNext().type != TokenType::DEL_RBRACE && !isAtEnd()) {
        // On propage le flag aux enfants de la liste
        node->add(parseElement(quoted));
    }

    expect(TokenType::DEL_RBRACE);
    return node;
}

ASTNode* Parser::parseElement(bool quoted) {
    Token t = showNext();

    if (t.type == TokenType::CORE_QUOTE) {
        acceptIt();
        // Appel récursif : on propage le fait que l'élément suivant est quoté
        return parseElement(true);
    }

    if (t.type == TokenType::DEL_LBRACE) {
        return parseSExpr(quoted);
    }

    return parseAtom(quoted);
}

ASTNode* Parser::parseAtom(bool quoted) {
    Token t = showNext();
    int l = t.line;
    int c = t.cursor;

    // --- 1. GESTION DES VALEURS SIMPLES (Littéraux) ---
    // Ces tokens NE SONT PAS des listes, on les retourne directement.

    if (t.type == TokenType::LIT_INT) {
        acceptIt();
        return new IntegerLit(std::stoi(t.value), l, c, quoted);
    }

    if (t.type == TokenType::LIT_FLOAT) {
        acceptIt();
        return new FloatLit(std::stod(t.value), l, c, quoted);
    }

    if (t.type == TokenType::LIT_STRING) {
        acceptIt(); // ON CONSOMME LE TOKEN
        return new StringLit(t.value, l, c, quoted);
    }

    if (t.type == TokenType::IDENT) {
        // Un identifiant seul (ex: une variable 'x') est un atome
        acceptIt();
        return new Identifier(t.value, l, c, quoted);
    }

    switch (t.type) {
        // --- FORMES SPÉCIALES (CORE) ---
        case TokenType::CORE_IF:
            return parseIf();
        case TokenType::CORE_SETQ:
            return parseSetq();
        case TokenType::CORE_LAMBDA:
            return parseLambda();
        case TokenType::CORE_PROGN:
            return parseProgn();
        case TokenType::CORE_LOAD:
            return parseLoad();
        case TokenType::CORE_PRINT:
            return parsePrint();

            // --- OPÉRATIONS ARITHMÉTIQUES ET LOGIQUES ---
        case TokenType::CALC_PLUS:
        case TokenType::CALC_MOINS:
        case TokenType::CALC_MULT:
        case TokenType::CALC_DIV:
        case TokenType::CALC_INF:
        case TokenType::CALC_SUP:
        case TokenType::CALC_EQ:
        case TokenType::CALC_ADREQ:
        case TokenType::CALC_NUMBERQ:
            return parseArithmetic();

            // --- PRIMITIVES DE MANIPULATION (MAIN) ---
        case TokenType::MAIN_CAR:
        case TokenType::MAIN_CDR:
        case TokenType::MAIN_CONS:
        case TokenType::MAIN_NULL:
        case TokenType::MAIN_ATOM:
            return parsePrimitiveCall();

            // --- CAS PAR DÉFAUT ---
            // Si c'est un IDENT (appel de fonction utilisateur) ou une autre SExpr
        default:
            return parseDefaultList(false);
    };
};

ASTNode* Parser::parseDefaultList(bool quoted) {
    int l = showNext().line;
    int c = showNext().cursor;

    // 1. On crée le nœud. On passe 'quoted' au constructeur
    // pour savoir si l'évaluateur doit traiter ça comme du code ou de la donnée.
    SExpr* listNode = new SExpr(l, c, quoted);

    // 2. Boucle de capture des éléments
    while (showNext().type != TokenType::DEL_RBRACE) {
        // Important : si la liste parente est 'quoted',
        // les enfants devraient probablement l'être aussi par propagation.
        listNode->add(parseElement(quoted));
    }

    // 3. Consommation du token de fermeture
    expect(TokenType::DEL_RBRACE);

    return listNode; // On retourne directement le pointeur
}

