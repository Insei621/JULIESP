#ifndef LEXER_TB_HPP
#define LEXER_TB_HPP

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

#include "../include/Lexer.h"
#include "../include/pch.h"
#include "../include/SemanticAnalyzer.h"
#include "../include/IR.h"
#include "../include/IRGenerator.h"
#include "../include/CGenerator.h"

/*
 * Affichage lisible des TokenType
 */
std::string testCode = R"(
§§ main.jsp
§§ Test de l'instruction LOAD

($ "math_utils.jsp")

(€ "--- Test du Load ---")

(: mon-age 20)

(? (est-majeur mon-age)
   (;
     (€ "Vous êtes majeur.")
     (€ "Le carré de votre âge est :")
     (€ (carre mon-age)))
   (€ "Vous êtes mineur."))
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

    // Génération d'IR
    IRGenerator gen;
    IRProgram ir = gen.generate(root);
    gen.dumpIR(ir); // Pour déboguer

    CGenerator cGen;
    std::filesystem::create_directories("output"); // Crée le dossier s'il n'existe pas
    cGen.generateToFile(ir, "output/output.c");


    //system("gcc -o output ../output/output.c");



}

#endif