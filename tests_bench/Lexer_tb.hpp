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
§§ test_07_load.jlsp
§§ Test du chargement de fichier externe ($)

($ "lib/math_lib.jlsp")

(€ "=== Test du Load ===")

(€ "carre(6) =")
(€ (carre 6))

(€ "cube(3) =")
(€ (cube 3))

(€ "abs_val(-5) =")
(€ (abs_val -5))

(€ "est_positif(10) =")
(€ (est_positif 10))

(€ "est_positif(-3) =")
(€ (est_positif -3))

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


    Parser parser(tokens);
    std::vector<ASTNode*> programNodes = parser.parseProgram();

    // On crée une S-Expression "racine" qui contient tout le programme
    SExpr* root = new SExpr(0, 0, false);
    root->add(new Primitive("progn", 0, 0, false)); // On simule un (progn ...)

    for (ASTNode* node : programNodes) {
        root->add(node);
    }

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