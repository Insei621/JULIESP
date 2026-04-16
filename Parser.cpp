//
// Created by keryan on 08/04/2026.
//

#include "Parser.h"
#include "pch.h"

Parser::Parser(std::vector<Token> t) : tokens(std::move(t)), current(0) {}

ASTNode* Parser::parse() {
    SExpr* root = new SExpr(showNext().line, showNext().cursor, false);

    while (!isAtEnd() && showNext().type != TokenType::END_OF_FILE) {
        std::cout << "DEBUG: Tentative sur le token " << showNext().value << std::endl; // AJOUTE ÇA

        ASTNode* element = parseElement();
        if (element) root->add(element);
    }
    return root;
}

/*ASTNode* Parser::parse() {
    // 1. On crée le conteneur racine
    SExpr* root = new SExpr(showNext().line, showNext().cursor, false);

    while (!isAtEnd() && showNext().type != TokenType::END_OF_FILE) {

        // On sauvegarde l'index actuel pour vérifier si on avance
        int lastIndex = current;

        ASTNode* element = parseElement();

        if (element != nullptr) {
            root->add(element);
        }

        // --- SÉCURITÉ ANTI-BOUCLE ---
        // Si l'index n'a pas bougé, c'est que parseElement a ignoré le token.
        // On force l'avancement pour ne pas boucler à l'infini.
        if (current == lastIndex) {
            std::cerr << "[Parser Error] Stuck at token: " << showNext().value << std::endl;
            acceptIt();
        }
    }

    return root;
}*/

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
    // 1. On consomme la parenthèse ouvrante
    expect(TokenType::DEL_LBRACE);

    // 2. On crée le nœud
    SExpr* node = new SExpr(showNext().line, showNext().cursor, quoted);

    // 3. Tant qu'on n'a pas la fermeture
    while (!isAtEnd() && showNext().type != TokenType::DEL_RBRACE) {
        int lastIndex = current;
        ASTNode* child = parseElement(quoted);

        if (child) {
            node->add(child);
        }

        // SÉCURITÉ : Si on n'a pas avancé, on force l'arrêt pour éviter le code 130
        if (current == lastIndex) {
            std::cerr << "ERREUR: Blocage sur " << showNext().value << std::endl;
            acceptIt();
        }
    }

    // 4. On consomme la parenthèse fermante
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
            // Si on arrive ici et que ce n'est PAS une parenthèse fermante ou un EOF
            // on risque la boucle infinie si on appelle parseDefaultList sans précaution.
            if (t.type == TokenType::DEL_RBRACE || t.type == TokenType::END_OF_FILE) {
                return nullptr;
            }
            // Si c'est un token vraiment inconnu, on lève une exception plutôt que de crasher
            throw std::runtime_error("Erreur Syntaxique : Token inattendu '" + t.value + "'");
    };
};

ASTNode* Parser::parseDefaultList(bool quoted) {
    int l = showNext().line;
    int c = showNext().cursor;

    SExpr* listNode = new SExpr(l, c, quoted);

    while (!isAtEnd() && showNext().type != TokenType::DEL_RBRACE) {
        ASTNode* child = parseElement(quoted);
        if (child != nullptr) {
            listNode->add(child);
        } else {
            // Si parseElement n'a rien renvoyé, on doit sortir ou avancer
            break;
        }
    }

    expect(TokenType::DEL_RBRACE);
    return listNode;
}

