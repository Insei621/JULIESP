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
§§ Test 1
(: x 42)
    (? (< x 50)
        (€ "Petit")
        (€ "Grand")
    )
    (+ 1 2 (* 3 4))



§§ Déclaration simple et calcul imbriqué
( : a 10 )
( : b ( + a ( * 5 2.5 ) ) )
( € b )

§§ Utilisation de littéraux différents
( : message "Resultat:" )
( € message b )

§§ Test du IF et des booléens
( : est_majeur µ )

( ? ( > a 18 )
    ( € "Acces autorise" )
    ( € "Acces refuse" ) )

§§ Vérification de type et d'égalité
( : est_un_nombre ( ° a ) )
( : sont_identiques ( ¤ a 10 ) )

§§ Création d'une liste avec cons (&)
( : ma_liste ( & 1 ( & 2 ( & 3 ù ) ) ) )

§§ Test du Quote pour éviter l'évaluation
( : liste_statique ²( 10 20 30 ) )

§§ Accès aux éléments
( : premier ( << ma_liste ) )
( : reste ( >> ma_liste ) )

§§ Vérification de liste vide
( ? ( | reste )
    ( € "Fin de liste" )
    ( € "Il en reste encore" ) )

§§ Définition d'une fonction anonyme
( : au_carre ( £ ( n ) ( * n n ) ) )

§§ Test du Progn (exécution séquentielle)
( ;
    ( € "Calcul en cours..." )
    ( : resultat ( au_carre 8 ) )
    ( € "Le carre de 8 est :" resultat )
)

(: nom "Bob")   §§ Le scanf ne prends que des ints, on ne peux pas lui demander de nom

( € "Bonjour " nom )

§§ Chargement d'un module externe
( $ "math_lib.jlsp" )

§!
   Ce bloc test les commentaires multi-lignes
   ainsi que les structures très profondes.
!§

( : complexe ( & ( + 1 2 ) ( & ( ? µ 42 0 ) ²( a b ( c d ) ) ) ) )

§§ Test des symboles isolés dans une liste
( € ( @ ( << complexe ) ) )

( : x 0.0001 ) §§ Test des petits décimaux


§§ Partie 2

§§ Test de l'addition et multiplication à n-arguments
( : gros_calcul ( + 1 2 3 4 5 6 7 8 9 ) )
( : poly ( + ( * 2 5 10 ) ( * 3 3 ) 1 ) ) §§ 100 + 9 + 1 = 110

§§ Test du Progn (;) avec beaucoup d'étapes
( ;
    ( : y 10 )
    ( : y ( + y 1 ) )
    ( : y ( * y 2 ) )
    ( € "Valeur finale de y :" y )
)

§§ Une liste qui contient une autre liste quotée
( : meta ²( 1 ²( a b ) 3 ) )

§§ Quoter une fonction pour la traiter comme de la donnée
( : commande ²( + 1 2 ) )
( : imbrication_quote ²( ²( ²( a ) ) ) )

§§ Listes vides dans des listes
( : vide ( & ( ) ( & ( ) ( ) ) ) )

§§ Symboles qui ressemblent à des opérateurs mais sont des identifiants
§§ Choix technique pour ce test: en lisp c'est possible mais dans juliesp on dira que non
§!
( : +++ 10 )
( : -_- "Humeur" )
( + +++ 5 )
!§

( ;
    ( ? µ ( € "Vrai" ) )  §§ IF sans ELSE
    ( € "Suite du code" )
)

§§ 1. Définition d'une variable globale
( : multiplicateur_universel 10 )

§§ 2. Définition d'une fonction avec £ (lambda) et ; (progn)
( : calculer_score
    ( £ (score_base)
        ( ;
            ( : bonus 50 )
            ( : total ( + score_base bonus ) )
            ( * total multiplicateur_universel )
        )
    )
)

§§ 3. Utilisation de structures de contrôle et d'affichage
( ? ( > multiplicateur_universel 0 )
    ( ;
        ( € "Calcul en cours..." )
        ( : resultat ( calculer_score 100 ) )
        ( € "Le résultat final est prêt." )
    )
    ( € "Erreur : Constante universelle invalide." )
)

§§ Partie 3

§§ Définition globale
( : x 100 )

( : tester_scopes
    ( £ ( a )
        ( ;
            ( : b ( + a x ) )
            ( € b )
        )
    )
)

§§ Appel de la fonction
( tester_scopes 50 )

§§ CE CI DOIT ÉCHOUER en sémantique si tu essaies d'accéder à 'b' ici :
§§ ( € b )



§§

( : n 0 ) §§ Variable globale n

( : factorielle
    ( £ ( n ) §§ Ce 'n' local doit masquer le 'n' global
        ( ? ( = n 0 )
            1
            ( * n ( factorielle ( - n 1 ) ) )
        )
    )
)

( : resultat ( factorielle 5 ) )
( € "Factorielle de 5 est :" )
( € resultat )


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

§§ Partie 4
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