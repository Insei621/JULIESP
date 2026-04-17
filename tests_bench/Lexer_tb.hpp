#ifndef LEXER_TB_HPP
#define LEXER_TB_HPP

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include "../include/Lexer.h"
#include "../include/pch.h"
#include "../include/SemanticAnalyzer.h"
#include "../include/IRGenerator.h"
#include "../include/IRPrinter.h"

/**
 * Affichage lisible des TokenType
 */
std::string testCode = R"(
§!
   Ce test vérifie si ton analyseur sémantique est "piégé"
   par le symbole ² (quote).
!§

( : x 10 )

( : test_quote
    ( £ ( l )
        ( ;
            ( : y ²x ) §§ Ici, y ne vaut pas 10, il vaut le SYMBOLE x.
            ( : z ²( + 1 2 ) ) §§ Ici, z ne vaut pas 3, il vaut la LISTE (+ 1 2).

            ( € "La variable x vaut :" )
            ( € x )

            ( € "Le symbole cité y est :" )
            ( € y )

            §§ Test de shadowing avec quote
            ( £ ( x )
                ( ;
                    ( : x 50 )
                    ( € "Ici x local vaut :" )
                    ( € x )
                    ( € "Mais ²x reste le symbole :" )
                    ( € ²x )
                )
            )
        )
    )
)

§§ Une liste qui contient des calculs et des quotes
( : ma_liste ( & ²test ( & ( + 5 5 ) ( & ²( << ( 1 2 ) ) ù ) ) ) )

( test_quote ma_liste )
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

    // Génération de l'IR
    IRGenerator irGen;
    IRModule module = irGen.generateModule(root);

    // Optimisations (optionnel)
    // IROptimizer opt;
    // opt.optimize(module.mainCFG);
    // ...

    // Création du dossier de sortie
    if (!fs::exists("CFG"))
        fs::create_directory("CFG");

    // Export console
    IRPrinter::printModule(module);

    // Export DOT (module complet avec toutes les fonctions)
    IRPrinter::exportModuleToDot(module, "CFG/module.dot");
    IRPrinter::generatePNG("CFG/module.dot", "CFG/module.png");
}





#endif