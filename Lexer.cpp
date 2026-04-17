//
// Created by kerya on 08/04/2026.
//

#include "Lexer.h"

Lexer::Lexer(std::string source)
    : source(std::move(source)),
      view(this->source),
      cursor(0),
      line(1)
{}

std::vector<Token> Lexer::tokenize() {
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
            throw std::runtime_error("Lexical error at line " + std::to_string(line) + " near: " + std::string(view.substr(0, 1)));
        }
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line, static_cast<int>(cursor)});
    reporting_erreurs(tokens);
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
    { std::regex(R"(^(\xC2\xB5|\xCE\xBC))"), TokenType::BOOL_TRUE, false },
    { std::regex(R"(^\xC3\xB9)"),             TokenType::BOOL_FALSE, false },
    { std::regex(R"(^\xC2\xB2)"),             TokenType::CORE_QUOTE, false },
    { std::regex(R"(^\xE2\x82\xAC)"),         TokenType::CORE_PRINT, false },
    { std::regex(R"(^\xC3\xA7)"),             TokenType::CORE_SCAN, false },
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
{ std::regex(R"(^([\x00-\x7F]|[\xC2-\xDF][\x80-\xBF]|\xE0[\xA0-\xBF][\x80-\xBF]))"), TokenType::UNKNOWN, false }};



/////////////////////
/*
// --- Constructeur ---
Lexer::Lexer(std::string source)
    : source(std::move(source)),
      view(this->source),
      cursor(0),
      line(1)
{}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!view.empty()) {
        bool matched = false;
        for (const auto& rule : rules) {
            std::cmatch match;
            if (std::regex_search(view.begin(), view.end(), match, rule.pattern, std::regex_constants::match_continuous)) {

                std::string lexeme = match.str();
                std::string value = lexeme;

                if (!rule.skip) {
                    // Nettoyage rapide (Strings et Chars)
                    if ((rule.type == TokenType::LIT_STRING || rule.type == TokenType::LIT_CHAR) && value.size() >= 2) {
                        value = value.substr(1, value.size() - 2);
                    }

                    tokens.push_back({rule.type, value, line, static_cast<int>(cursor)});
                }

                advance(lexeme); // On avance TOUJOURS de la taille réelle du match
                matched = true;
                break;
            }
        }

        if (!matched) {
            throw std::runtime_error("Lexical error near: \"" + std::string(view.substr(0, 10)) + "\"");
        }
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line, static_cast<int>(cursor)});
    reporting_erreurs(tokens);
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
            std::cerr
                << "[Erreur lexicale] caractère inattendu '"
                << tok.value
                << "' [l"
                << tok.line
                << "; c"
                << tok.cursor
                << "]"
                << std::endl;
        }
    }

    if (has_error) {
        std::cerr << "Analyse lexicale échouée." << std::endl;
    }
}

const std::vector<Lexer::Rule> Lexer::rules = {
// --- Espaces ---
    { std::regex(R"(^[ \t\r\n]+)"), TokenType::DEL_SPACE, true },

    // --- Commentaires ---
    { std::regex(R"(^§§[^\n]*)"), TokenType::COM_LINE, true },
    { std::regex(R"(^§![\s\S]*?!§)"), TokenType::COM_BLOCK, true },

    // --- Littéraux string et char ---
    { std::regex(R"(^"([^"\\]|\\.)*")"), TokenType::LIT_STRING, false },
    { std::regex(R"(^'([^'\\]|\\.)')"), TokenType::LIT_CHAR, false},

    // --- Délimiteurs ---
    { std::regex(R"(^\()"), TokenType::DEL_LBRACE, false },
    { std::regex(R"(^\))"), TokenType::DEL_RBRACE, false },

    // --- Symboles doubles (AVANT les symboles simples) ---
    { std::regex(R"(^<<)"), TokenType::MAIN_CAR, false },
    { std::regex(R"(^>>)"), TokenType::MAIN_CDR, false },

    // --- Symboles UTF-8 (Bien ancrés avec ^) ---
    { std::regex(u8R"(^[μµ])"),          TokenType::BOOL_TRUE, false }, // Gère les deux types de mu/micro
    { std::regex(R"(^ù)"),             TokenType::BOOL_FALSE, false },
    { std::regex(R"(^²)"),             TokenType::CORE_QUOTE, false },
    { std::regex(R"(^€)"),             TokenType::CORE_PRINT, false}, // Corrigé : ^€
    { std::regex(R"(^ç)"),             TokenType::CORE_SCAN, false},  // Corrigé : ^ç
    { std::regex(R"(^£)"),             TokenType::CORE_LAMBDA, false },
    { std::regex(R"(^¤)"),             TokenType::CALC_ADREQ, false },
    { std::regex(R"(^°)"),             TokenType::CALC_NUMBERQ, false },

    // --- Symboles simples ---
    { std::regex(R"(^\?)"),            TokenType::CORE_IF, false },
    { std::regex(R"(^:)"),             TokenType::CORE_SETQ, false },
    { std::regex(R"(^;)"),             TokenType::CORE_PROGN, false }, // Corrigé : ^;
    { std::regex(R"(^\$)"),            TokenType::CORE_LOAD, false },
    { std::regex(R"(^&)"),             TokenType::MAIN_CONS, false },
    { std::regex(R"(^\|)"),            TokenType::MAIN_NULL, false },
    { std::regex(R"(^@)"),             TokenType::MAIN_ATOM, false },

    { std::regex(R"(^\+)"),            TokenType::CALC_PLUS, false },
    { std::regex(R"(^-)"),             TokenType::CALC_MOINS, false },
    { std::regex(R"(^\*)"),            TokenType::CALC_MULT, false },
    { std::regex(R"(^/)"),             TokenType::CALC_DIV, false },
    { std::regex(R"(^<)"),             TokenType::CALC_INF, false },
    { std::regex(R"(^>)"),             TokenType::CALC_SUP, false },
    { std::regex(R"(^=)"),             TokenType::CALC_EQ, false },

    // --- Littéraux numériques (Float avant Int !) ---
    { std::regex(R"(^[0-9]+\.[0-9]+)"), TokenType::LIT_FLOAT, false },
    { std::regex(R"(^[0-9]+)"),         TokenType::LIT_INT, false },

    // --- Identificateurs ---
    { std::regex(R"(^[A-Za-z_][A-Za-z0-9_]*)"), TokenType::IDENT, false }, // Ajout de _ et chiffres

    // --- Caractère inatendu (A la toute fin !) ---
    { std::regex(R"(^.)"),              TokenType::UNKNOWN, false} // ^. pour ne pas tout manger d'un coup
};
*/