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
§§ test_10_listes_fonctions.jlsp
§§ Test des listes dans des fonctions

§§ --- Fonction : premier élément ---
(: premier (£ (lst) (<< lst)))

§§ --- Fonction : deuxième élément ---
(: deuxieme (£ (lst) (<< (>> lst))))

§§ --- Fonction : troisième élément ---
(: troisieme (£ (lst) (<< (>> (>> lst)))))

§§ --- Fonction : construire une paire et lire ses éléments ---
(: fst (£ (paire) (<< paire)))
(: snd (£ (paire) (<< (>> paire))))

§§ --- Fonction : liste est-elle un atome ou une paire ? ---
(: describe (£ (x)
    (? (@ x)
       (€ "C est un atome")
       (€ "C est une liste"))))

§§ --- Fonction : liste est-elle vide ? ---
(: est_vide (£ (lst)
    (? (| lst)
       (€ "Liste vide")
       (€ "Liste non vide"))))

§§ --- Programme principal ---
(: nums (& 10 (& 20 (& 30 ²()))))


§!
(€ "=== Accès par position ===")
(€ "Element 1 :")
(€ (premier nums))
(€ "Element 2 :")
(€ (deuxieme nums))
(€ "Element 3 :")
(€ (troisieme nums))

(€ "=== Test paires ===")
(: p (& 100 200))
(€ "fst(p) =")
(€ (fst p))
(€ "snd(p) =")
(€ (snd p))

(€ "=== Atom? et null? dans fonctions ===")
(describe 42)
(describe nums)
(est_vide ²())
(est_vide nums)

§§ --- Test enchaînement cons ---
(€ "=== Construction et déconstruction ===")
(: a 1)
(: b 2)
(: c 3)
(: liste3 (& a (& b (& c ²()))))
(€ "Liste (a b c), tete :")
(€ (<< liste3))
(€ "Queue de la liste, tete :")
(€ (<< (>> liste3)))
!§
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