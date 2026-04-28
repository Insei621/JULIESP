//
// IRGenerator.h — Visiteur qui transforme l'AST en IR
//
// Ce visiteur hérite de Visitor (comme PrettyPrinter et SemanticAnalyzer)
// et parcourt l'AST pour produire un IRProgram.
//
// PRINCIPE DE FONCTIONNEMENT :
//
// Chaque méthode visit() peut avoir besoin de "retourner" une valeur
// à son appelant (ex: visit(IntegerLit*) produit une constante "42"
// que visit(SExpr*) doit pouvoir récupérer pour l'utiliser comme argument).
//
// En C++, les méthodes virtuelles ne retournent pas de valeur facilement
// via polymorphisme. On utilise donc un membre "lastResult_" qui agit
// comme registre de communication entre les niveaux de récursion.
//   → Après chaque appel à node->accept(this), lastResult_ contient
//     l'opérande produite par ce nœud.
//

#ifndef COMPILATEUR_JULIESP_IRGENERATOR_H
#define COMPILATEUR_JULIESP_IRGENERATOR_H

#include "Visitor.h"
#include "IR.h"
#include "AST.h"
#include "pch.h"

class IRGenerator : public Visitor {
public:
    IRGenerator();

    // Point d'entrée : on lui donne la racine de l'AST, il retourne le programme IR
    IRProgram generate(ASTNode* root);

    // Dump lisible pour déboguer (affiche l'IR en pseudo-code)
    void dumpIR(const IRProgram& program, std::ostream& out = std::cout) const;

    // =========================================================================
    // Méthodes de visite — implémentées dans IRGenerator.cpp
    // =========================================================================

    void visit(SExpr*       node) override;
    void visit(Identifier*  node) override;
    void visit(Primitive*   node) override;
    void visit(IntegerLit*  node) override;
    void visit(FloatLit*    node) override;
    void visit(StringLit*   node) override;
    void visit(BoolLit*     node) override;
    void visit(CharLit*     node) override;

private:
    // =========================================================================
    // État interne du générateur
    // =========================================================================

    IRProgram program_;         // Le programme IR en cours de construction

    // Pointeur vers le bloc courant dans lequel on émet les instructions.
    // Au départ c'est &program_.mainBlock.
    // Quand on entre dans un lambda, on crée un nouveau IR_Block et on
    // pointe ici dedans. Quand on en sort, on restaure le pointeur.
    IR_Block* currentBlock_;

    // "Registre" de communication entre visit() et son appelant.
    // Après node->accept(this), lastResult_ contient l'opérande produite.
    IROperand lastResult_;

    // Compteur pour générer des noms uniques de temporaires : t0, t1, t2...
    int tempCounter_;

    // Compteur pour générer des noms uniques de labels : L0, L1, L2...
    int labelCounter_;

    // Compteur pour générer des noms uniques de fonctions : __fn0, __fn1...
    int funcCounter_;

    // =========================================================================
    // Utilitaires privés
    // =========================================================================

    // Génère un nouveau nom de temporaire et l'émet dans le bloc courant
    // Exemple : newTemp(IRType::INT) émet "int t0;" et retourne "t0"
    IROperand newTemp(IRType type);

    // Génère un nouveau nom de label unique
    std::string newLabel(const std::string& prefix = "L");

    // Génère un nouveau nom de fonction unique
    std::string newFuncName(const std::string& hint = "");

    // Émet une instruction dans le bloc courant
    void emit(IRInstruction instr);

    // =========================================================================
    // Handlers pour les formes spéciales
    // Ces méthodes sont appelées depuis visit(SExpr*) selon le premier enfant
    // =========================================================================

    // (if cond then else?)
    IROperand handleIf(SExpr* node);

    // (setq name value)
    IROperand handleSetq(SExpr* node);

    // (lambda (params...) body...)
    IROperand handleLambda(SExpr* node);
    IROperand handleLambdaWithName(SExpr* node, const std::string& name);

    // (progn expr1 expr2 ...)
    IROperand handleProgn(SExpr* node);

    // (print expr)
    IROperand handlePrint(SExpr* node);

    // (scan var)
    IROperand handleScan(SExpr* node);

    // Arithmétique : (+, -, *, /, <, >, ==, <=>=)
    IROperand handleArithmetic(SExpr* node, const std::string& op);

    // Primitives Lisp : car, cdr, cons, null?, atom?, number?
    // (on les compile en appels de fonctions C runtime)
    IROperand handlePrimitive(SExpr* node);

    // Appel de fonction générique : (funcName arg1 arg2 ...)
    IROperand handleCall(SExpr* node);

    // =========================================================================
    // Utilitaire : détermine le IRType d'un nœud AST
    // (heuristique simple, pas de vrai système de types ici)
    // =========================================================================
    IRType inferType(ASTNode* node);

    // Dump d'un seul bloc (utilisé par dumpIR)
    void dumpBlock(const IR_Block& block, std::ostream& out) const;

    // Convertit une IRInstruction en string lisible
    std::string instrToString(const IRInstruction& instr) const;

    // Table de types : nom de variable → IRType
    // Remplie au fur et à mesure des setq
    std::unordered_map<std::string, IRType> typeTable_;
};

#endif //COMPILATEUR_JULIESP_IRGENERATOR_H
