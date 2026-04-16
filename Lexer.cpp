//
// Created by kerya on 08/04/2026.
//

#include "Lexer.h"

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

            if (std::regex_search(
                    view.begin(),
                    view.end(),
                    match,
                    rule.pattern,
                    std::regex_constants::match_continuous)) {

                std::string lexeme = match.str();

                if (!rule.skip) {
                    tokens.push_back({
                        rule.type,
                        lexeme,
                        line,
                        static_cast<int>(cursor)
                    });
                }

                advance(lexeme);
                matched = true;
                break;
                    }
        }

        if (!matched) {
            throw std::runtime_error(
                "Lexical error near: \"" +
                std::string(view.substr(0, 10)) + "\""
            );
        }
    }

    tokens.push_back({
        TokenType::END_OF_FILE,
        "",
        line,
        static_cast<int>(cursor)
    });
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

    // --- Délimiteurs ---
    { std::regex(R"(^\()"), TokenType::DEL_LBRACE, false },
    { std::regex(R"(^\))"), TokenType::DEL_RBRACE, false },

    // --- Symboles doubles ---
    { std::regex(R"(^(<<))"), TokenType::MAIN_CAR, false },
    { std::regex(R"(^(>>))"), TokenType::MAIN_CDR, false },

    // --- Symboles UTF-8 ---
    { std::regex(R"(^¤)"), TokenType::CALC_ADREQ, false },
    { std::regex(R"(^£)"), TokenType::CORE_LAMBDA, false },
    { std::regex(R"(^°)"), TokenType::CALC_NUMBERQ, false },
    { std::regex(R"(^μ)"), TokenType::BOOL_TRUE, false },
    { std::regex(R"(^ù)"), TokenType::BOOL_FALSE, false },
    { std::regex(R"(^²)"), TokenType::CORE_QUOTE, false },

    // --- Symboles simples ---
    { std::regex(R"(^\?)"), TokenType::CORE_IF, false },
    { std::regex(R"(^:)"), TokenType::CORE_SETQ, false },
    { std::regex(R"(;^)"), TokenType::CORE_PROGN, false },
    { std::regex(R"(^\$)"), TokenType::CORE_LOAD, false },
    { std::regex(R"(^&)"), TokenType::MAIN_CONS, false },
    { std::regex(R"(^\|)"), TokenType::MAIN_NULL, false },
    { std::regex(R"(^@)"), TokenType::MAIN_ATOM, false },

    { std::regex(R"(^\+)"), TokenType::CALC_PLUS, false },
    { std::regex(R"(^-)"), TokenType::CALC_MOINS, false },
    { std::regex(R"(^\*)"), TokenType::CALC_MULT, false },
    { std::regex(R"(^/)"), TokenType::CALC_DIV, false },
    { std::regex(R"(^<)"), TokenType::CALC_INF, false },
    { std::regex(R"(^>)"), TokenType::CALC_SUP, false },
    { std::regex(R"(^=)"), TokenType::CALC_EQ, false },

    // --- Littéraux ---
    { std::regex(R"(^[0-9]+\.[0-9]+)"), TokenType::LIT_FLOAT, false },
    { std::regex(R"(^[0-9]+)"), TokenType::LIT_INT, false },
    { std::regex(R"(^"([^"\\]|\\.)*")"), TokenType::LIT_STRING, false },
    { std::regex(R"(^'([^'\\]|\\.)')"), TokenType::LIT_CHAR, false},

    // --- Identificateurs ---
    { std::regex(R"(^[A-Za-z]+)"), TokenType::IDENT, false },

    // --- Caractère inatendu ---
    {std::regex(R"(.)"), TokenType::UNKNOWN, false}
};