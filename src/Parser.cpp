//
// Created by keryan on 08/04/2026.
//

#include "../include/Parser.h"
#include "../include/pch.h"

Parser::Parser(std::vector<Token> t) : tokens(std::move(t)), current(0) {}

ASTNode* Parser::parse() {
    std::cout << "\033[1;34m[Parsing]\033[0m Lancement de l'analyse..." << std::endl;
    // On capture le premier token pour le nœud racine
    Token first = showNext();
    SExpr* root = new SExpr(first.line, first.cursor, false);

    try {
        int elementCount = 0;
        while (!isAtEnd() && showNext().type != TokenType::END_OF_FILE) {
            ASTNode* element = parseElement();
            if (element) {
                root->add(element);
                elementCount++;
            }
        }

        // Message de succès final
        std::cout << "\033[1;32m[Succès]\033[0m Analyse syntaxique terminée (" << elementCount << " expressions)." << std::endl;
        return root;

    } catch (const std::exception& e) {
        throw; // On relance l'erreur pour qu'elle soit attrapée plus haut
    }
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
        std::string msg = "\033[1;31m[Erreur Syntaxique]\033[0m Ligne " + std::to_string(actualToken.line) +
                                  " : attendu type " + std::to_string(static_cast<int>(expectedType)) +
                                  " mais trouvé '" + actualToken.value + "'.";

        throw std::runtime_error(msg);
    }
}

bool Parser::isAtEnd() const {
    return current >= tokens.size();
}

/// --- 2.Parsing
ASTNode* Parser::parseSExpr(bool quoted) {
    expect(TokenType::DEL_LBRACE); // Consomme '('

    // Cas de la liste vide ()
    if (showNext().type == TokenType::DEL_RBRACE) {
        int l = showNext().line;
        int c = showNext().cursor;
        expect(TokenType::DEL_RBRACE);
        return new SExpr(l, c, quoted);
    }

    // Si on n'est pas dans un 'quote, on cherche les formes spéciales
    if (!quoted) {
        TokenType type = showNext().type;
        switch (type) {
            case TokenType::CORE_IF:     return parseIf();
            case TokenType::CORE_SETQ:   return parseSetq();
            case TokenType::CORE_LAMBDA: return parseLambda();
            case TokenType::CORE_PROGN:  return parseProgn();
            case TokenType::CORE_LOAD:   return parseLoad();
            case TokenType::CORE_PRINT:  return parsePrint();

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

            case TokenType::MAIN_CAR:
            case TokenType::MAIN_CDR:
            case TokenType::MAIN_CONS:
            case TokenType::MAIN_NULL:
            case TokenType::MAIN_ATOM:
                return parsePrimitiveCall();

            default:
                break; // Sort du switch pour aller vers parseDefaultList
        }
    }

    // Si c'est un quote ou un appel de fonction standard (ex: (foo 1 2))
    return parseDefaultList(quoted);
}

ASTNode* Parser::parseElement(bool quoted) {
    Token t = showNext();

    if (t.type == TokenType::CORE_QUOTE) {
        acceptIt(); // 1. On "consomme" le ² (il disparaît du flux)
        // 2. On rappelle parseElement EN FORÇANT le booléen à true
        return parseElement(true);
    }

    if (t.type == TokenType::DEL_LBRACE) {
        return parseSExpr(quoted); // Transmet le "pinceau" à la liste
    }

    return parseAtom(quoted); // Transmet le "pinceau" à l'atome
}

/*
ASTNode* Parser::parseElement(bool quoted) {
    Token t = showNext();

    // 1. Gestion du Quote (on propage l'état)
    if (t.type == TokenType::CORE_QUOTE) {
        acceptIt();
        return parseElement(true);
    }

    // 2. Si c'est une parenthèse, on délègue à parseSExpr
    if (t.type == TokenType::DEL_LBRACE) {
        return parseSExpr(quoted);
    }

    // 3. Sinon, c'est obligatoirement un Atome (nombre, identifiant, etc.)
    return parseAtom(quoted);
}
*/

ASTNode* Parser::parseAtom(bool quoted) {
    Token t = showNext();
    int l = t.line;
    int c = t.cursor;

    // RÈGLE D'OR : On avance systématiquement avant de retourner l'atome
    acceptIt();

    if (t.type == TokenType::LIT_INT)    return new IntegerLit(std::stoi(t.value), l, c, quoted);
    if (t.type == TokenType::LIT_FLOAT)  return new FloatLit(std::stod(t.value), l, c, quoted);
    if (t.type == TokenType::LIT_STRING) return new StringLit(t.value, l, c, quoted);

    // 2. NOUVEAU : Littéraux Booléens (µ et ù)
    if (t.type == TokenType::BOOL_TRUE)  return new BoolLit(true, l, c, quoted);
    if (t.type == TokenType::BOOL_FALSE) return new BoolLit(false, l, c, quoted);


    // Identifiants, symboles (+, -, if isolés), etc.
    return new Identifier(t.value, l, c, quoted);
}

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

