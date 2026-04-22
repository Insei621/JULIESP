//
// IR.h — Représentation Intermédiaire (IR) du compilateur
//
// Ce fichier définit TOUTES les structures de données qui forment l'IR.
// Chaque struct représente une instruction atomique, proche de ce qu'on
// écrira plus tard en C.
//
// On utilise std::variant (C++17) pour regrouper tous les types
// d'instructions dans un seul type IRInstruction sans hiérarchie de classes.
//

#ifndef COMPILATEUR_JULIESP_IR_H
#define COMPILATEUR_JULIESP_IR_H

#include "pch.h"

// =============================================================================
// SECTION 1 : Les types de valeurs
// =============================================================================
//
// On a besoin de représenter le type d'une variable IR pour que le générateur
// C sache s'il doit écrire "int", "float", "char*", etc.
//

enum class IRType {
    INT,
    FLOAT,
    CHAR,
    STRING,
    BOOL,
    VOID,       // Pour les instructions sans valeur de retour (print, etc.)
    UNKNOWN     // Avant résolution de type (on peut l'étendre plus tard)
};

// Utilitaire : convertit un IRType en chaîne C pour le générateur
inline std::string irTypeToC(IRType t) {
    switch (t) {
        case IRType::INT:    return "int";
        case IRType::FLOAT:  return "float";
        case IRType::CHAR:   return "char";
        case IRType::STRING: return "char*";
        case IRType::BOOL:   return "int"; // En C, les booléens sont des ints
        case IRType::VOID:   return "void";
        default:             return "/*unknown*/";
    }
}

// =============================================================================
// SECTION 2 : Les "opérandes" — ce qu'une instruction peut consommer
// =============================================================================
//
// Une opérande est soit :
//   - Une variable temporaire   : "t0", "t1", ...
//   - Un identifiant utilisateur : "x", "ma_var", ...
//   - Une constante littérale   : "42", "3.14", "'a'", ...
//
// On utilise juste un std::string pour simplifier — le générateur C
// copiera la valeur telle quelle dans le code généré.
//

using IROperand = std::string;

// =============================================================================
// SECTION 3 : Les instructions IR
// =============================================================================
//
// Chaque struct = un type d'instruction.
// On les liste en suivant les constructions de ton langage (Parser.h).
//

// --- 3.1 Assignation d'une constante ---
// Exemple : (setq x 42)  →  int x = 42;
// Aussi utilisé pour les temporaires : t0 = 3.14
struct IR_Assign {
    IRType  type;   // Le type de la variable
    IROperand dest; // La variable destination (ex: "t0" ou "x")
    IROperand src;  // La valeur source (ex: "42" ou "t1")
};

// --- 3.2 Opération arithmétique binaire ---
// Exemple : (+ t0 t1)  →  t2 = t0 + t1;
struct IR_BinOp {
    IRType    type;  // Type du résultat
    IROperand dest;  // Variable qui reçoit le résultat
    IROperand left;  // Opérande gauche
    std::string op;  // L'opérateur : "+", "-", "*", "/", "<", ">", "=="
    IROperand right; // Opérande droite
};

// --- 3.3 Appel de fonction (lambda ou primitive C) ---
// Exemple : (ma-fn a b)  →  t3 = ma_fn(a, b);
// Si la fonction est void (ex: print), dest sera vide.
struct IR_Call {
    IRType    type;                  // Type de retour
    IROperand dest;                  // Variable qui reçoit le résultat (vide si void)
    std::string funcName;            // Nom de la fonction appelée
    std::vector<IROperand> args;     // Liste des arguments
};

// --- 3.4 Déclaration d'une fonction ---
// Exemple : (lambda (x y) ...)  →  int __fn0(int x, int y) { ... }
// On stocke le nom généré, les paramètres et le corps (liste d'instructions).
// NOTE : le corps (body) sera rempli récursivement par l'IRGenerator.
struct IR_FuncDecl {
    IRType    returnType;
    std::string name;                              // Nom généré : "__fn0", ou le nom setq
    std::vector<std::pair<IRType, std::string>> params; // (type, nom) de chaque param
    // Le corps sera une liste d'instructions — on le déclare après IRInstruction
    // (voir plus bas, on utilise un forward via shared_ptr<IR_Block>)
};

// --- 3.5 Saut conditionnel (if) ---
// Exemple : (if cond then else)
//   devient :  if (cond) goto L_then; else goto L_else;
//              L_then: ...
//              L_else: ...
// On stocke les labels générés et la condition.
struct IR_CondJump {
    IROperand condition; // La variable booléenne évaluée (ex: "t2")
    std::string labelTrue;  // Label du bloc "then"
    std::string labelFalse; // Label du bloc "else"
};

// --- 3.6 Label (point de saut) ---
// Exemple :  L_then:
struct IR_Label {
    std::string name; // Nom du label (ex: "L_then_0")
};

// --- 3.7 Saut inconditionnel ---
// Exemple : goto L_end;
// Utilisé pour sauter la branche else après le then.
struct IR_Jump {
    std::string label;
};

// --- 3.8 Instruction return ---
// Exemple : return t5;
struct IR_Return {
    IROperand value; // Valeur retournée (peut être vide pour void)
};

// --- 3.9 Print (I/O) ---
// Exemple : (print x)  →  printf("%d\n", x);
// On garde le type pour choisir le bon format printf.
struct IR_Print {
    IRType    type;
    IROperand value;
};

// --- 3.10 Scan (I/O) ---
// Exemple : (scan x)  →  scanf("%d", &x);
struct IR_Scan {
    IRType    type;
    IROperand dest;
};

// =============================================================================
// SECTION 4 : Le type variant — une instruction IR
// =============================================================================
//
// std::variant est comme une union typée et sûre.
// Un IRInstruction EST EXACTEMENT UN des types listés.
// On peut inspecter lequel avec std::holds_alternative<T>()
// et accéder à la valeur avec std::get<T>().
//

using IRInstruction = std::variant<
    IR_Assign,
    IR_BinOp,
    IR_Call,
    IR_FuncDecl,
    IR_CondJump,
    IR_Label,
    IR_Jump,
    IR_Return,
    IR_Print,
    IR_Scan
>;

// =============================================================================
// SECTION 5 : Le bloc IR — une séquence d'instructions
// =============================================================================
//
// Un IR_Block représente une séquence linéaire d'instructions :
// le programme principal, ou le corps d'une fonction.
// On l'enveloppe dans un struct pour pouvoir l'étendre plus tard
// (ex: ajouter un nom, des métadonnées...).
//

struct IR_Block {
    std::string name;                          // "main", "__fn0", ...
    std::vector<IRInstruction> instructions;   // Les instructions dans l'ordre

    void emit(IRInstruction instr) {
        instructions.push_back(std::move(instr));
    }
};

// =============================================================================
// SECTION 6 : Le programme IR complet
// =============================================================================
//
// Un IRProgram contient :
//   - Les fonctions déclarées (chacune avec son propre IR_Block comme corps)
//   - Le bloc "main" (le programme principal)
//
// NOTE : On stocke les fonctions séparément du main pour que le générateur C
// puisse les écrire avant le main() (obligation en C : déclarer avant d'appeler).
//

struct IRProgram {
    // Les fonctions déclarées (lambdas, etc.)
    // Chaque entrée = (déclaration, corps)
    std::vector<std::pair<IR_FuncDecl, IR_Block>> functions;

    // Le bloc principal (ce qui s'exécute au top-level)
    IR_Block mainBlock;

    IRProgram() : mainBlock(IR_Block{"main", {}}) {}
};

#endif //COMPILATEUR_JULIESP_IR_H
