//
// CGenerator.h — Générateur de code C à partir de l'IR
//
// Ce module prend un IRProgram (produit par IRGenerator) et écrit
// un fichier .c valide et compilable sur stdout ou dans un fichier.
//
// ARCHITECTURE :
//   - generateProgram()  : point d'entrée, écrit tout le fichier
//   - emitFunctions()    : écrit chaque fonction lambda avant le main()
//   - emitMain()         : écrit la fonction main() C
//   - emitBlock()        : traduit un IR_Block en instructions C
//   - emitInstruction()  : traduit une IRInstruction en une ligne C
//
// STRATÉGIE DE DÉCLARATION DES VARIABLES :
//   En C, on ne peut pas écrire "int t0 = ..." au milieu d'un bloc si
//   on cible C89/C99 strict. On fait donc une passe préalable (collectDecls)
//   qui collecte tous les temporaires du bloc, puis on les déclare en tête.
//

#ifndef COMPILATEUR_JULIESP_CGENERATOR_H
#define COMPILATEUR_JULIESP_CGENERATOR_H

#include "IR.h"
#include "pch.h"

class CGenerator {
public:
    // Écrit le fichier C complet dans le flux donné (par défaut : stdout)
    void generateProgram(const IRProgram& program, std::ostream& out = std::cout);

    // Écrit le fichier C dans un fichier sur disque
    void generateToFile(const IRProgram& program, const std::string& filename);

private:
    // =========================================================================
    // Sections du fichier généré
    // =========================================================================

    // Écrit le prologue : #include, prototypes forward des fonctions
    void emitPrologue(const IRProgram& program, std::ostream& out);

    // Écrit toutes les fonctions (lambdas)
    void emitFunctions(const IRProgram& program, std::ostream& out);

    // Écrit la fonction main()
    void emitMain(const IRProgram& program, std::ostream& out);

    // =========================================================================
    // Émission d'un bloc et de ses instructions
    // =========================================================================

    // Traduit un IR_Block complet (avec la passe de déclaration en tête)
    void emitBlock(const IR_Block& block, std::ostream& out, int indentLevel = 1);

    size_t emitIfElse(const std::vector<IRInstruction>& instrs, size_t i, std::ostream& out, int indentLevel);

    // Traduit une seule IRInstruction en C
    void emitInstruction(const IRInstruction& instr, std::ostream& out, int indentLevel);

    // =========================================================================
    // Passe de collecte des déclarations
    // =========================================================================

    // Collecte tous les temporaires (t0, t1...) et variables déclarées dans un bloc.
    // Retourne un vecteur (type, nom) pour chaque variable à déclarer.
    // On utilise un set pour éviter les doublons (un temporaire peut être dest
    // de plusieurs instructions — ex: dans les branches if).
    std::vector<std::pair<IRType, std::string>> collectDecls(
        const IR_Block& block,
        const std::unordered_set<std::string>& used);  // ← paramètre ajouté
    // =========================================================================
    // Utilitaires d'émission
    // =========================================================================

    // Retourne la chaîne d'indentation (2 espaces par niveau)
    std::string indent(int level);

    // Retourne le format printf/scanf pour un IRType
    std::string printfFormat(IRType type);

    // Retourne true si une chaîne ressemble à un temporaire (commence par 't'
    // suivi de chiffres uniquement)
    bool isTemp(const std::string& name);

    // Retourne true si une chaîne est une constante littérale
    // (commence par un chiffre, guillemet, apostrophe, ou signe -)
    bool isLiteral(const std::string& name);

    std::string irTypeToC(IRType type);
    std::unordered_set<std::string> collectUsedFunctions(const IRProgram& program);

};

#endif //COMPILATEUR_JULIESP_CGENERATOR_H
