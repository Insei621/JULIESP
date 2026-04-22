/*
 * exemple_usage.cpp — Comment brancher IRGenerator + CGenerator dans ton main()
 *
 * Ce fichier montre comment utiliser les deux nouveaux composants,
 * et illustre ce que chaque étape du pipeline produit.
 */

// ============================================================================
// UTILISATION DANS TON MAIN
// ============================================================================

/*
    // --- Étape 1 : Lexer + Parser (déjà fait) ---
    Lexer lexer(source_code);
    auto tokens = lexer.tokenize();

    Parser parser(std::move(tokens));
    ASTNode* ast = parser.parse();

    // --- Étape 2 : Analyse sémantique (déjà fait) ---
    SemanticAnalyzer sem;
    sem.analyze(ast);

    // --- Étape 3 : Génération d'IR (nouveau) ---
    IRGenerator irGen;
    IRProgram ir = irGen.generate(ast);

    // Optionnel : dump de l'IR pour déboguer
    irGen.dumpIR(ir, std::cout);

    // --- Étape 4 : Génération de code C (nouveau) ---
    CGenerator cGen;
    cGen.generateToFile(ir, "output.c");

    // Optionnel : compiler automatiquement avec gcc
    system("gcc -o output output.c");
*/

// ============================================================================
// EXEMPLE CONCRET — ce que le pipeline produit
// ============================================================================
//
// Source JulieSP :
//
//   (setq x 10)
//   (setq y 20)
//   (setq result (+ x y))
//   (print result)
//
// ─── IRGenerator produit (dumpIR) : ───────────────────────────────────────
//
//   MAIN {
//     int x = 10;
//     int y = 20;
//     int t0 = x + y;
//     int result = t0;
//     PRINT(result);
//   }
//
// ─── CGenerator produit (output.c) : ──────────────────────────────────────
//
//   /* Code généré automatiquement — ne pas éditer */
//
//   #include <stdio.h>
//   #include <stdlib.h>
//   #include <string.h>
//   #include "lisp_runtime.h"
//
//   int main(void) {
//       /* --- déclarations --- */
//       int x;
//       int y;
//       int t0;
//       int result;
//
//       x = 10;
//       y = 20;
//       t0 = x + y;
//       result = t0;
//       printf("%d\n", result);
//       return 0;
//   }
//
// ===========================================================================
//
// Source JulieSP (avec if) :
//
//   (if (> x 5) (print x) (print 0))
//
// ─── IRGenerator produit : ────────────────────────────────────────────────
//
//   MAIN {
//     int t0 = x > 5;         ← la condition
//     if (t0) goto L_then_0; else goto L_else_1;
//     L_then_0:
//     PRINT(x);
//     int t1 = ;               ← résultat du then (vide ici car print est void)
//     goto L_end_2;
//     L_else_1:
//     PRINT(0);
//     L_end_2:
//   }
//
// ─── CGenerator produit : ─────────────────────────────────────────────────
//
//   int main(void) {
//       int t0;
//       int t1;
//
//       t0 = x > 5;
//       if (t0) goto L_then_0; else goto L_else_1;
//   L_then_0: ;
//       printf("%d\n", x);
//       t1 = ;
//       goto L_end_2;
//   L_else_1: ;
//       printf("%d\n", 0);
//   L_end_2: ;
//       return 0;
//   }
//
// ===========================================================================
//
// Source JulieSP (avec lambda) :
//
//   (setq add (lambda (a b) (+ a b)))
//   (print (add 3 4))
//
// ─── IRGenerator produit : ────────────────────────────────────────────────
//
//   FUNCTION __fn0(unknown a, unknown b) -> unknown {
//     int t0 = a + b;
//     return t0;
//   }
//
//   MAIN {
//     add = __fn0;             ← setq stocke le nom de la fonction
//     int t1 = add(3, 4);
//     PRINT(t1);
//   }
//
// ─── CGenerator produit : ─────────────────────────────────────────────────
//
//   /* Prototypes */
//   int __fn0(int a, int b);
//
//   int __fn0(int a, int b) {
//       int t0;
//       t0 = a + b;
//       return t0;
//   }
//
//   int main(void) {
//       int t1;
//       t1 = add(3, 4);
//       printf("%d\n", t1);
//       return 0;
//   }
