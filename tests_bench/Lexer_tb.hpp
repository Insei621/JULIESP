#ifndef LEXER_TB_HPP
#define LEXER_TB_HPP

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include "../include/Lexer.h"
#include "../include/pch.h"
#include "../include/SemanticAnalyzer.h"

/**
 * Affichage lisible des TokenType
 */
std::string testCode = R"(

§§ Création d'une liste complexe
( : ma_liste ( & 10 ( & 20 ( & 30 ù ) ) ) )

( : analyser_liste
    ( £ ( l )
        ( ;
            ( ? ( | l ) §§ Test null?
                ( € "Liste vide" )
                ( ;
                    ( € ( << l ) )   §§ Test car
                    ( € ( @ ( << l ) ) ) §§ Test atom?
                    ( : reste ( >> l ) )  §§ Test cdr
                    ( € reste )
                )
            )
        )
    )
)

( analyser_liste ma_liste )
        )";


/**
 * Fonction de test principale
 */
inline void run_test() {
    std::cout << "=== Code test ===\n";
    std::cout << testCode << std::endl;

    // Lexage
    Lexer lexer(testCode);
    std::vector<Token> tokens = lexer.tokenize();

    // Parsing
    Parser parser(tokens);
    ASTNode* root = parser.parse();

    /*PrettyPrinter printer;  // Impression de l'AST dans le terminal
    root->accept(&printer);
    std::cout << std::endl;*/

    // Création du graphe de l'AST
    std::ofstream dotFile("AST_Graphe/ast.dot");//Impression de l'AST dans un fichier pour pouvoir visualiser le graphique
    dotFile << "digraph G {\n";
    GraphvizVisitor gv(dotFile);
    root->accept(&gv);
    dotFile << "}\n";
    dotFile.close();
    system("dot -Tpng AST_Graphe/ast.dot -o AST_Graphe/ast.png");

    // Analyse sémantique
    SemanticAnalyzer semanticAnalyzer;
    semanticAnalyzer.analyze(root);

}

#endif