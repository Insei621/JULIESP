//
// Created by kerya on 08/04/2026.
//

#include "../include/Lexer.h"

Lexer::Lexer(std::string source)
    : source(std::move(source)),
      view(this->source),
      cursor(0),
      line(1)
{}

std::vector<Token> Lexer::tokenize() {
    std::cout << "\033[1;34m[Lexage]\033[0m Lancement de l'analyse...";
    std::vector<Token> tokens;

    while (!view.empty()) {
        bool matched = false;
        for (const auto& rule : rules) {
            std::cmatch match;
            // Utilisation de match_continuous pour forcer l'ancrage au début de la vue
            if (std::regex_search(view.data(), view.data() + view.size(), match, rule.pattern, std::regex_constants::match_continuous)) {

                std::string lexeme = match.str();
                std::string value = lexeme;

                if (!rule.skip) {
                    if ((rule.type == TokenType::LIT_STRING || rule.type == TokenType::LIT_CHAR) && value.size() >= 2) {
                        value = value.substr(1, value.size() - 2);
                    }
                    tokens.push_back({rule.type, value, line, static_cast<int>(cursor)});
                }

                advance(lexeme);
                matched = true;
                break;
            }
        }

        if (!matched) {
            // En cas d'échec total, on lève une exception sur le caractère fautif
            throw std::runtime_error("\033[1;31m[Erreur Lexicale]\033[0m Ligne " + std::to_string(line) + " : caractère '" + std::string(view.substr(0, 1)) + "' non reconnu.");        }
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line, static_cast<int>(cursor)});
    reporting_erreurs(tokens);

    afficherTokens(tokens);
    std::cout << "\033[1;32m[Succès]\033[0m Analyse lexicale terminée." << std::endl;
    return tokens;
}

void Lexer::advance(std::string_view matched) {
    for (char c : matched) {
        if (c == '\n') line++;
    }
    cursor += matched.size();
    view.remove_prefix(matched.size());
}

void Lexer::reporting_erreurs(const std::vector<Token>& tokens) const {
    bool has_error = false;
    for (const auto& tok : tokens) {
        if (tok.type == TokenType::UNKNOWN) {
            has_error = true;
            std::cerr << "[Erreur lexicale] caractère inattendu '" << tok.value
                      << "' [l" << tok.line << "; c" << tok.cursor << "]" << std::endl;
        }
    }
    if (has_error) std::cerr << "Analyse lexicale échouée." << std::endl;
}

// --- Définition des règles avec support UTF-8 explicite ---
const std::vector<Lexer::Rule> Lexer::rules = {
    // --- Espaces & Commentaires ---
    { std::regex(R"(^[ \t\r\n]+)"), TokenType::DEL_SPACE, true },
    // Retrait du u8 ici :
    { std::regex(R"(^§§[^\n]*)"), TokenType::COM_LINE, true },
    { std::regex(R"(^§![\s\S]*?!§)"), TokenType::COM_BLOCK, true },

    // --- Littéraux ---
    { std::regex(R"(^"([^"\\]|\\.)*")"), TokenType::LIT_STRING, false },
    { std::regex(R"(^'([^'\\]|\\.)')"), TokenType::LIT_CHAR, false},

    // Délimiteurs & Symboles doubles
    { std::regex(R"(^\()"), TokenType::DEL_LBRACE, false },
    { std::regex(R"(^\))"), TokenType::DEL_RBRACE, false },
    { std::regex(R"(^<<)"), TokenType::MAIN_CAR, false },
    { std::regex(R"(^>>)"), TokenType::MAIN_CDR, false },

    // --- Symboles UTF-8 (Codés par octets Hex pour éviter les collisions) ---
    { std::regex(R"(^(\xC2\xB5|\xCE\xBC))"),  TokenType::BOOL_TRUE, false },
    { std::regex(R"(^\xC3\xB9)"),             TokenType::BOOL_FALSE, false },
    { std::regex(R"(^\xC2\xB2)"),             TokenType::CORE_QUOTE, false },
    { std::regex(R"(^\xE2\x82\xAC)"),         TokenType::CORE_PRINT, false },
    { std::regex(R"(^ç)"),                    TokenType::CORE_SCAN, false },
    { std::regex(R"(^\xC2\xA3)"),             TokenType::CORE_LAMBDA, false },
    { std::regex(R"(^\xC2\xA4)"),             TokenType::CALC_ADREQ, false },
    { std::regex(R"(^\xC2\xB0)"),             TokenType::CALC_NUMBERQ, false },

    // Symboles simples ASCII
    { std::regex(R"(^\?)"), TokenType::CORE_IF, false },
    { std::regex(R"(^:)"),  TokenType::CORE_SETQ, false },
    { std::regex(R"(^;)"),  TokenType::CORE_PROGN, false },
    { std::regex(R"(^\$)"), TokenType::CORE_LOAD, false },
    { std::regex(R"(^&)"),  TokenType::MAIN_CONS, false },
    { std::regex(R"(^\|)"), TokenType::MAIN_NULL, false },
    { std::regex(R"(^@)"),  TokenType::MAIN_ATOM, false },
    { std::regex(R"(^\+)"), TokenType::CALC_PLUS, false },
    { std::regex(R"(^-)"),  TokenType::CALC_MOINS, false },
    { std::regex(R"(^\*)"), TokenType::CALC_MULT, false },
    { std::regex(R"(^/)"),  TokenType::CALC_DIV, false },
    { std::regex(R"(^<)"),  TokenType::CALC_INF, false },
    { std::regex(R"(^>)"),  TokenType::CALC_SUP, false },
    { std::regex(R"(^=)"),  TokenType::CALC_EQ, false },

    // Nombres
    { std::regex(R"(^[0-9]+\.[0-9]+)"), TokenType::LIT_FLOAT, false },
    { std::regex(R"(^[0-9]+)"),         TokenType::LIT_INT, false },

    // Identificateurs
    { std::regex(R"(^[A-Za-z_][A-Za-z0-9_]*)"), TokenType::IDENT, false },

    // Catch-all UTF-8 (évite de découper un caractère multi-octets en plusieurs tokens UNKNOWN)
{ std::regex(R"(^([\x00-\x7F]|[\xC2-\xDF][\x80-\xBF]|\xE0[\xA0-\xBF][\x80-\xBF]))"), TokenType::UNKNOWN, false }
};

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {

        // --- Identifiants / Littéraux ---
        case TokenType::IDENT:        return "IDENT";
        case TokenType::LIT_INT:      return "LIT_INT";
        case TokenType::LIT_FLOAT:    return "LIT_FLOAT";

        // --- Booléens ---
        case TokenType::BOOL_TRUE:    return "BOOL_TRUE";
        case TokenType::BOOL_FALSE:   return "BOOL_FALSE";

        // --- Espaces / commentaires ---
        case TokenType::DEL_SPACE:    return "DEL_SPACE";
        case TokenType::COM_LINE:     return "COM_LINE";
        case TokenType::COM_BLOCK:    return "COM_BLOCK";

        // --- Délimiteurs ---
        case TokenType::DEL_LBRACE:   return "DEL_LBRACE";
        case TokenType::DEL_RBRACE:   return "DEL_RBRACE";

        // --- Noyau Lisp ---
        case TokenType::CORE_IF:      return "CORE_IF";
        case TokenType::CORE_SETQ:    return "CORE_SETQ";
        case TokenType::CORE_PROGN:   return "CORE_PROGN";
        case TokenType::CORE_LOAD:    return "CORE_LOAD";
        case TokenType::CORE_LAMBDA:  return "CORE_LAMBDA";
        case TokenType::CORE_QUOTE:   return "CORE_QUOTE";

        // --- Calcul ---
        case TokenType::CALC_PLUS:    return "CALC_PLUS";
        case TokenType::CALC_MOINS:   return "CALC_MOINS";
        case TokenType::CALC_MULT:    return "CALC_MULT";
        case TokenType::CALC_DIV:     return "CALC_DIV";
        case TokenType::CALC_INF:     return "CALC_INF";
        case TokenType::CALC_SUP:     return "CALC_SUP";
        case TokenType::CALC_EQ:      return "CALC_EQ";
        case TokenType::CALC_ADREQ:   return "CALC_ADREQ";
        case TokenType::CALC_NUMBERQ:return "CALC_NUMBERQ";

        // --- Listes ---
        case TokenType::MAIN_CAR:     return "MAIN_CAR";
        case TokenType::MAIN_CDR:     return "MAIN_CDR";
        case TokenType::MAIN_CONS:    return "MAIN_CONS";
        case TokenType::MAIN_NULL:    return "MAIN_NULL";
        case TokenType::MAIN_ATOM:    return "MAIN_ATOM";

            // --- À ajouter dans ton switch ---
        case TokenType::LIT_STRING:   return "LIT_STRING";
        case TokenType::LIT_CHAR:     return "LIT_CHAR";
        case TokenType::CORE_PRINT:   return "CORE_PRINT"; // Pour le symbole €
        case TokenType::CORE_SCAN:    return "CORE_SCAN";

        // --- Fin / erreur ---
        case TokenType::END_OF_FILE:  return "EOF";
        case TokenType::UNKNOWN:      return "UNKNOWN";

        default:                      return "???";
    }
}


void Lexer::afficherTokens(const std::vector<Token>& tokens) const {

            std::cout << "\n\033[1m" // Gras pour l'en-tête
              << std::left
              << std::setw(20) << "TYPE"
              << std::setw(25) << "VALUE"
              << std::setw(8)  << "LINE"
              << "COL"
              << "\033[0m\n"; // Reset

    std::cout << std::string(60, '-') << '\n';

    for (const auto& t : tokens) {
        // On évite d'afficher le token de fin de fichier dans le tableau pour plus de clarté
        if (t.type == TokenType::END_OF_FILE) continue;

        std::cout << std::left
                  << std::setw(20) << tokenTypeToString(t.type)
                  << std::setw(25) << (t.value.empty() ? "\"\"" : t.value)
                  << std::setw(8)  << t.line
                  << t.cursor << '\n';
    }
}
