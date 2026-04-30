//
// CGenerator.cpp — Implémentation du générateur de code C
//

#include "../include/CGenerator.h"

// =============================================================================
// Point d'entrée principal
// =============================================================================

void CGenerator::generateProgram(const IRProgram& program, std::ostream& out) {
    emitPrologue(program, out);
    emitFunctions(program, out);
    emitMain(program, out);
}

void CGenerator::generateToFile(const IRProgram& program, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("CGenerator : impossible d'ouvrir le fichier : " + filename);
    }
    generateProgram(program, file);
}

// =============================================================================
// SECTION 1 : Prologue
// =============================================================================
//
// Le prologue contient :
//   1. Les #include standard
//   2. L'inclusion du runtime Lisp (fonctions car, cdr, cons, etc.)
//   3. Les prototypes (forward declarations) de toutes les fonctions
//      générées — obligatoire en C si une fonction appelle une autre
//      définie plus loin dans le fichier.
//

void CGenerator::emitPrologue(const IRProgram& program, std::ostream& out) {
    out << "/* Code généré automatiquement — ne pas éditer */\n\n";

    // Includes standard
    out << "#include <stdio.h>\n";
    out << "#include <stdlib.h>\n";
    out << "#include <string.h>\n";
    out << "#include \"lisp_runtime.h\"\n";  // Notre runtime Lisp
    out << "\n";
    // Génération de prototypes fais par claude mais ça me parait pas utile
/*
    // Forward declarations de toutes les fonctions générées.
    // Sans ça, si __fn1 appelle __fn0 défini après lui, le compilateur C
    // se plaindrait d'un appel à une fonction inconnue.
    if (!program.functions.empty()) {
        out << "// --- Prototypes --- \n";
        for (const auto& [decl, body] : program.functions) {
            out << irTypeToC(decl.returnType) << " " << decl.name << "(";
            for (size_t i = 0; i < decl.params.size(); ++i) {
                // Si le type est UNKNOWN, on met "int" par défaut
                IRType t = decl.params[i].first;
                out << (t == IRType::UNKNOWN ? "int" : irTypeToC(t));
                out << " " << decl.params[i].second;
                if (i + 1 < decl.params.size()) out << ", ";
            }
            out << ");\n";
        }
        out << "\n";
    }*/
}

// =============================================================================
// SECTION 2 : Fonctions (lambdas)
// =============================================================================
//
// Chaque fonction est écrite AVANT le main() pour éviter les problèmes
// d'ordre de définition en C.
//
// Format produit :
//
//   int __fn0(int x, int y) {
//       /* déclarations des temporaires */
//       int t0;
//       /* corps */
//       t0 = x + y;
//       return t0;
//   }
//

void CGenerator::emitFunctions(const IRProgram& program, std::ostream& out) {
    auto used = collectUsedFunctions(program);  // ← doit être LA PREMIÈRE ligne

    for (const auto& [decl, body] : program.functions) {
        if (!used.count(decl.name)) continue;

        bool hasReturn = false;
        for (const auto& instr : body.instructions) {
            if (std::holds_alternative<IR_Return>(instr)) { hasReturn = true; break; }
        }
        out << (hasReturn ? "int" : "void");        out << " " << decl.name << "(";

        if (decl.params.empty()) {
            out << "void";
        } else {
            for (size_t i = 0; i < decl.params.size(); ++i) {
                IRType t = decl.params[i].first;
                out << (t == IRType::UNKNOWN ? "int" : irTypeToC(t));
                out << " " << decl.params[i].second;
                if (i + 1 < decl.params.size()) out << ", ";
            }
        }

        out << ") {\n";
        emitBlock(body, out, 1);
        out << "}\n\n";
    }
}
// =============================================================================
// SECTION 3 : Main
// =============================================================================

void CGenerator::emitMain(const IRProgram& program, std::ostream& out) {
    out << "int main(void) {\n";
    emitBlock(program.mainBlock, out, 1);
    // Si le bloc ne se termine pas déjà par un return, on ajoute return 0
    bool hasReturn = false;
    for (const auto& instr : program.mainBlock.instructions) {
        if (std::holds_alternative<IR_Return>(instr)) { hasReturn = true; break; }
    }
    if (!hasReturn) {
        out << indent(1) << "return 0;\n";
    }
    out << "}\n";
}

// =============================================================================
// SECTION 4 : Émission d'un bloc
// =============================================================================
//
// Un bloc se traduit en deux temps :
//   1. On collecte toutes les variables à déclarer (temporaires + variables
//      setq qui ne sont pas des paramètres).
//   2. On émet les déclarations, puis les instructions.
//

void CGenerator::emitBlock(const IR_Block& block, std::ostream& out, int indentLevel) {

    // Calcule les variables vivantes
    std::unordered_set<std::string> used;
    for (const auto& instr : block.instructions) {
        if (std::holds_alternative<IR_Assign>(instr)) {
            const auto& a = std::get<IR_Assign>(instr);
            if (!isLiteral(a.src)) used.insert(a.src);
        }
        else if (std::holds_alternative<IR_BinOp>(instr)) {
            const auto& b = std::get<IR_BinOp>(instr);
            if (!isLiteral(b.left))  used.insert(b.left);
            if (!isLiteral(b.right)) used.insert(b.right);
        }
        else if (std::holds_alternative<IR_Call>(instr)) {
            for (const auto& arg : std::get<IR_Call>(instr).args)
                if (!isLiteral(arg)) used.insert(arg);
        }
        else if (std::holds_alternative<IR_Print>(instr)) {
            const auto& p = std::get<IR_Print>(instr);
            if (!isLiteral(p.value)) used.insert(p.value);
        }
        else if (std::holds_alternative<IR_CondJump>(instr)) {
            const auto& j = std::get<IR_CondJump>(instr);
            if (!isLiteral(j.condition)) used.insert(j.condition);
        }
        else if (std::holds_alternative<IR_Return>(instr)) {
            const auto& r = std::get<IR_Return>(instr);
            if (!isLiteral(r.value)) used.insert(r.value);
        }
    }

    // Déclarations filtrées
    auto decls = collectDecls(block, used);  // ← passe used
    if (!decls.empty()) {
        for (const auto& [type, name] : decls)
            out << indent(indentLevel) << irTypeToC(type) << " " << name << ";\n";
        out << "\n";
    }

    // Instructions — filtre les assignations vers variables mortes
    const auto& instrs = block.instructions;
    size_t i = 0;
    while (i < instrs.size()) {
        if (std::holds_alternative<IR_Assign>(instrs[i])) {
            const auto& a = std::get<IR_Assign>(instrs[i]);
            if (!used.count(a.dest)) { ++i; continue; }  // ← ignore si mort
        }
        if (std::holds_alternative<IR_CondJump>(instrs[i])) {
            i = emitIfElse(instrs, i, out, indentLevel);
        } else {
            emitInstruction(instrs[i], out, indentLevel);
            ++i;
        }
    }
}

size_t CGenerator::emitIfElse(const std::vector<IRInstruction>& instrs,
                               size_t i, std::ostream& out, int indentLevel) {
    const auto& cj = std::get<IR_CondJump>(instrs[i]);
    std::string labelElse = cj.labelFalse;
    std::string labelEnd;
    ++i;

    // Saute le label then
    if (i < instrs.size() && std::holds_alternative<IR_Label>(instrs[i])) ++i;

    // --- Émet la branche THEN directement ---
    out << indent(indentLevel) << "if (" << cj.condition << ") {\n";
    while (i < instrs.size() && !std::holds_alternative<IR_Jump>(instrs[i])) {
        if (std::holds_alternative<IR_CondJump>(instrs[i])) {
            i = emitIfElse(instrs, i, out, indentLevel + 1);
        } else if (std::holds_alternative<IR_Assign>(instrs[i])) {
            const auto& a = std::get<IR_Assign>(instrs[i]);
            if (!a.src.empty()) emitInstruction(instrs[i], out, indentLevel + 1);
            ++i;
        } else {
            emitInstruction(instrs[i], out, indentLevel + 1);
            ++i;
        }
    }
    if (i < instrs.size()) {
        labelEnd = std::get<IR_Jump>(instrs[i]).label; // récupère le label de fin
        ++i; // saute le IR_Jump
    }
    out << indent(indentLevel) << "}";

    // Saute le label else
    if (i < instrs.size() && std::holds_alternative<IR_Label>(instrs[i])) ++i;

    // --- Vérifie si la branche else est vide ---
    // Elle est vide si on tombe directement sur le label de fin
    bool elseEmpty = (i < instrs.size() &&
                      std::holds_alternative<IR_Label>(instrs[i]) &&
                      std::get<IR_Label>(instrs[i]).name == labelEnd);

    if (!elseEmpty) {
        out << " else {\n";
        // Émet la branche ELSE jusqu'au label de fin
        while (i < instrs.size()) {
            // Stop si on atteint le label de fin
            if (std::holds_alternative<IR_Label>(instrs[i]) &&
                std::get<IR_Label>(instrs[i]).name == labelEnd) break;

            if (std::holds_alternative<IR_CondJump>(instrs[i])) {
                i = emitIfElse(instrs, i, out, indentLevel + 1);
            } else if (std::holds_alternative<IR_Assign>(instrs[i])) {
                const auto& a = std::get<IR_Assign>(instrs[i]);
                if (!a.src.empty()) emitInstruction(instrs[i], out, indentLevel + 1);
                ++i;
            } else {
                emitInstruction(instrs[i], out, indentLevel + 1);
                ++i;
            }
        }
        out << indent(indentLevel) << "}";
    }
    out << "\n";

    // Saute le label de fin
    if (i < instrs.size() && std::holds_alternative<IR_Label>(instrs[i])) ++i;

    return i;
}

// =============================================================================
// SECTION 5 : Collecte des déclarations
// =============================================================================
//
// On parcourt toutes les instructions et on collecte les "dest" qui sont :
//   - Des temporaires (t0, t1...) — toujours déclarés ici
//   - Des variables utilisateur (setq) — déclarées ici aussi
//
// On exclut :
//   - Les constantes littérales (42, "hello", etc.)
//   - Les chaînes vides (instructions void)
//
// On utilise une ordered_map pour garder l'ordre de première apparition
// tout en évitant les doublons.
//

std::vector<std::pair<IRType, std::string>> CGenerator::collectDecls(
    const IR_Block& block,
    const std::unordered_set<std::string>& used) {

    std::vector<std::pair<IRType, std::string>> result;
    std::unordered_map<std::string, bool> seen;

    auto tryAdd = [&](IRType type, const std::string& name) {
        if (name.empty()) return;
        if (isLiteral(name)) return;
        if (seen.count(name)) return;
        if (!used.count(name)) return;  // ← filtre les variables mortes
        seen[name] = true;
        result.push_back({ type, name });
    };

    for (const auto& instr : block.instructions) {
        if (std::holds_alternative<IR_Assign>(instr)) {
            const auto& a = std::get<IR_Assign>(instr);
            tryAdd(a.type, a.dest);
        }
        else if (std::holds_alternative<IR_BinOp>(instr)) {
            const auto& b = std::get<IR_BinOp>(instr);
            tryAdd(b.type, b.dest);
        }
        else if (std::holds_alternative<IR_Call>(instr)) {
            const auto& c = std::get<IR_Call>(instr);
            if (!c.dest.empty()) tryAdd(c.type, c.dest);
        }
    }
    return result;
}// =============================================================================
// SECTION 6 : Traduction d'une instruction
// =============================================================================
//
// C'est ici qu'on mappe chaque IRInstruction vers sa syntaxe C.
// On utilise std::holds_alternative + std::get pour inspecter le variant.
//
// NOTE sur les labels en C :
//   Un label doit être suivi d'une instruction. Si le label est en fin de
//   bloc, on ajoute un ";" vide : "L_end_0: ;"
//

void CGenerator::emitInstruction(const IRInstruction& instr, std::ostream& out, int indentLevel) {

    // --- IR_Assign : dest = src; ---
    if (std::holds_alternative<IR_Assign>(instr)) {
        const auto& a = std::get<IR_Assign>(instr);
        // On n'émet PAS le type ici — les déclarations sont déjà en tête de bloc
        out << indent(indentLevel) << a.dest << " = " << a.src << ";\n";
        return;
    }

    // --- IR_BinOp : dest = left op right; ---
    if (std::holds_alternative<IR_BinOp>(instr)) {
        const auto& b = std::get<IR_BinOp>(instr);
        out << indent(indentLevel)
            << b.dest << " = " << b.left << " " << b.op << " " << b.right << ";\n";
        return;
    }

    // --- IR_Call : dest = funcName(arg1, arg2, ...); ---
    //              ou funcName(arg1, arg2, ...);  si dest est vide (void)
    if (std::holds_alternative<IR_Call>(instr)) {
        const auto& c = std::get<IR_Call>(instr);
        out << indent(indentLevel);
        if (!c.dest.empty()) {
            out << c.dest << " = ";
        }
        out << c.funcName << "(";
        for (size_t i = 0; i < c.args.size(); ++i) {
            out << c.args[i];
            if (i + 1 < c.args.size()) out << ", ";
        }
        out << ");\n";
        return;
    }

    // --- IR_CondJump : if (cond) goto L_true; else goto L_false; ---
    if (std::holds_alternative<IR_CondJump>(instr)) {
        const auto& j = std::get<IR_CondJump>(instr);
        out << indent(indentLevel)
            << "if (" << j.condition << ") goto " << j.labelTrue
            << "; else goto " << j.labelFalse << ";\n";
        return;
    }

    // --- IR_Label : L_name: ; ---
    // Le "; " vide est obligatoire en C99/C11 : un label ne peut pas être
    // la dernière instruction d'un bloc (undefined behavior sinon).
    if (std::holds_alternative<IR_Label>(instr)) {
        const auto& l = std::get<IR_Label>(instr);
        // Les labels ne sont pas indentés (convention de lisibilité)
        out << l.name << ": ;\n";
        return;
    }

    // --- IR_Jump : goto L_name; ---
    if (std::holds_alternative<IR_Jump>(instr)) {
        out << indent(indentLevel) << "goto " << std::get<IR_Jump>(instr).label << ";\n";
        return;
    }

    // --- IR_Return : return value; ou return; ---
    if (std::holds_alternative<IR_Return>(instr)) {
        const auto& r = std::get<IR_Return>(instr);
        out << indent(indentLevel) << "return";
        if (!r.value.empty()) out << " " << r.value;
        out << ";\n";
        return;
    }

    // --- IR_Print : printf(format, value); ---
    //
    // On choisit le format selon le type :
    //   INT/BOOL → %d
    //   FLOAT    → %f
    //   CHAR     → %c
    //   STRING   → %s
    //   UNKNOWN  → %d  (défaut)
    //
    if (std::holds_alternative<IR_Print>(instr)) {
        const auto& p = std::get<IR_Print>(instr);
        out << indent(indentLevel)
            << "printf(\"" << printfFormat(p.type) << "\\n\", " << p.value << ");\n";
        return;
    }

    // --- IR_Scan : scanf(format, &dest); ---
    //
    // ATTENTION : pour les strings, scanf lit dans un buffer existant.
    // On ne peut pas faire scanf("%s", &str) — on passe str directement
    // (char* est déjà une adresse).
    //
    if (std::holds_alternative<IR_Scan>(instr)) {
        const auto& s = std::get<IR_Scan>(instr);
        out << indent(indentLevel) << "scanf(\"" << printfFormat(s.type) << "\", ";
        if (s.type == IRType::STRING) {
            out << s.dest;  // char* est déjà un pointeur
        } else {
            out << "&" << s.dest;  // Adresse pour les scalaires
        }
        out << ");\n";
        return;
    }

    // --- IR_FuncDecl inline : ne devrait pas apparaître dans un bloc ---
    // (Les fonctions sont stockées dans program_.functions, pas dans un bloc)
    if (std::holds_alternative<IR_FuncDecl>(instr)) {
        out << indent(indentLevel) << "/* [IR_FuncDecl inline — ignoré] */\n";
        return;
    }

    out << indent(indentLevel) << "/* [instruction inconnue] */\n";
}

// =============================================================================
// Utilitaires
// =============================================================================

std::string CGenerator::indent(int level) {
    return std::string(level * 4, ' '); // 4 espaces par niveau
}

std::string CGenerator::printfFormat(IRType type) {
    switch (type) {
        case IRType::INT:    return "%d";
        case IRType::FLOAT:  return "%f";
        case IRType::CHAR:   return "%c";
        case IRType::STRING: return "%s";
        case IRType::BOOL:   return "%d";
        default:             return "%d"; // UNKNOWN → on tente int
    }
}

bool CGenerator::isTemp(const std::string& name) {
    // Un temporaire commence par 't' suivi uniquement de chiffres : t0, t12, ...
    if (name.empty() || name[0] != 't') return false;
    return std::all_of(name.begin() + 1, name.end(), ::isdigit);
}

bool CGenerator::isLiteral(const std::string& name) {
    if (name.empty()) return true;
    // Commence par un chiffre → constante numérique : 42, 3.14f
    if (std::isdigit(name[0])) return true;
    // Commence par '-' suivi d'un chiffre → nombre négatif
    if (name[0] == '-' && name.size() > 1 && std::isdigit(name[1])) return true;
    // Commence par '"' → string littérale
    if (name[0] == '"') return true;
    // Commence par '\'' → char littéral
    if (name[0] == '\'') return true;
    // Vaut "1" ou "0" → bool littéral
    if (name == "1" || name == "0") return true;
    // Commentaire placeholder (quoted list, etc.)
    if (name.rfind("/*", 0) == 0) return true;
    return false;
}

std::string CGenerator::irTypeToC(IRType type) {
    switch (type) {
        case IRType::INT:    return "int";
        case IRType::BOOL:   return "int";
        case IRType::FLOAT:  return "float";
        case IRType::STRING: return "char*";
        case IRType::VOID:   return "void";
        default:             return "int"; // Sécurité : au pire, on met int
    }
}

std::unordered_set<std::string> CGenerator::collectUsedFunctions(const IRProgram& program) {
    std::unordered_set<std::string> used;
    std::queue<std::string> toVisit;

    // Collecte les appels depuis main
    for (const auto& instr : program.mainBlock.instructions) {
        if (std::holds_alternative<IR_Call>(instr)) {
            toVisit.push(std::get<IR_Call>(instr).funcName);
        }
    }

    // Propagation transitive : si carre appelle autre chose, on l'inclut aussi
    while (!toVisit.empty()) {
        std::string fname = toVisit.front(); toVisit.pop();
        if (used.count(fname)) continue;
        used.insert(fname);

        // Cherche les appels dans le corps de cette fonction
        for (const auto& [decl, body] : program.functions) {
            if (decl.name == fname) {
                for (const auto& instr : body.instructions) {
                    if (std::holds_alternative<IR_Call>(instr)) {
                        toVisit.push(std::get<IR_Call>(instr).funcName);
                    }
                }
            }
        }
    }
    return used;
}
