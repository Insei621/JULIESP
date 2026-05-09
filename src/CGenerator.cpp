//
// CGenerator.cpp — Implémentation du générateur de code C
//

#include "../include/CGenerator.h"

#include <complex>

void CGenerator::generateProgram(const IRProgram& program, std::ostream& out) {
    // Dans generateProgram :
    program_ = program;
    emitPrologue(program, out);
    emitGlobals(program, out);
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

void CGenerator::emitPrologue(const IRProgram& program, std::ostream& out) {
    out << "/* Code généré automatiquement — ne pas éditer */\n\n";

    out << "#include <stdio.h>\n";
    out << "#include <stdlib.h>\n";
    out << "#include <string.h>\n";
    out << "#include \"./juliesp_runtime.h\"\n";
    out << "\n";

    // Dans emitPrologue, après le runtime :
    if (!program.functions.empty()) {
        auto used = collectUsedFunctions(program);
        out << "/* --- Prototypes --- */\n";
        for (const auto& [decl, body] : program.functions) {
            if (!used.count(decl.name)) continue;

            // On utilise systématiquement irTypeToC pour le retour et les paramètres
            out << irTypeToC(decl.returnType) << " " << decl.name << "(";

            if (decl.params.empty()) {
                out << "void";
            } else {
                for (size_t i = 0; i < decl.params.size(); ++i) {
                    out << irTypeToC(decl.params[i].first) << " " << decl.params[i].second;
                    if (i + 1 < decl.params.size()) out << ", ";
                }
            }
            out << ");\n";
        }
        out << "\n";
    }


}

void CGenerator::emitGlobals(const IRProgram& program, std::ostream& out) {
    auto used = computeUsed(program.mainBlock);  // ← utilise computeUsed
    auto decls = collectDecls(program.mainBlock, used);

    if (decls.empty()) return;

    out << "/* --- Variables globales --- */\n";
    for (const auto& [type, name] : decls)
        out << irTypeToC(type) << " " << name << ";\n";
    out << "\n";
}

void CGenerator::emitFunctions(const IRProgram& program, std::ostream& out) {
    auto used = collectUsedFunctions(program);

    for (const auto& [decl, body] : program.functions) {
        if (!used.count(decl.name)) continue;

        // Signature de la fonction
        out << irTypeToC(decl.returnType) << " " << decl.name << "(";

        if (decl.params.empty()) {
            out << "void";
        } else {
            for (size_t i = 0; i < decl.params.size(); ++i) {
                // On écrit : type nom_parametre
                out << irTypeToC(decl.params[i].first) << " " << decl.params[i].second;
                if (i + 1 < decl.params.size()) out << ", ";
            }
        }
        out << ") {\n";

        // 1. On émet le corps de la fonction
        emitBlock(body, out, 1);

        // 2. On vérifie si la dernière instruction était déjà un retour
        bool dejaUnReturn = false;
        if (!body.instructions.empty()) {
            // On regarde si la variante active de la dernière instruction est IR_Return
            if (std::holds_alternative<IR_Return>(body.instructions.back())) {
                dejaUnReturn = true;
            }
        }

        // 3. On ne rajoute le return de sécurité QUE si nécessaire
        if (!dejaUnReturn && decl.returnType != IRType::VOID) {
            out << indent(1) << "return 0; // NIL par défaut\n";
        }

        out << "}\n\n";
    }
}

void CGenerator::emitMain(const IRProgram& program, std::ostream& out) {
    out << "int main(void) {\n";

    auto used = computeUsed(program.mainBlock);  // ← utilise computeUsed

    const auto& instrs = program.mainBlock.instructions;
    size_t i = 0;
    while (i < instrs.size()) {
        if (std::holds_alternative<IR_Assign>(instrs[i])) {
            const auto& a = std::get<IR_Assign>(instrs[i]);
            if (!used.count(a.dest)) { ++i; continue; }
        }
        else if (std::holds_alternative<IR_BinOp>(instrs[i])) {
            const auto& b = std::get<IR_BinOp>(instrs[i]);
            if (!used.count(b.dest)) { ++i; continue; }
        }
        else if (std::holds_alternative<IR_Call>(instrs[i])) {
            const auto& c = std::get<IR_Call>(instrs[i]);
            if (!c.dest.empty() && !used.count(c.dest)) { ++i; continue; }
        }

        if (std::holds_alternative<IR_CondJump>(instrs[i])) {
            i = emitIfElse(instrs, i, out, 1, used);
        } else {
            emitInstruction(instrs[i], out, 1);
            ++i;
        }
    }

    out << indent(1) << "return 0;\n";
    out << "}\n";
}

void CGenerator::emitBlock(const IR_Block& block, std::ostream& out, int indentLevel) {
    auto used = computeUsed(block);  // ← utilise computeUsed

    auto decls = collectDecls(block, used);
    if (!decls.empty()) {
        for (const auto& [type, name] : decls)
            out << indent(indentLevel) << irTypeToC(type) << " " << name << ";\n";
        out << "\n";
    }

    const auto& instrs = block.instructions;
    size_t i = 0;
    while (i < instrs.size()) {
        if (std::holds_alternative<IR_Assign>(instrs[i])) {
            const auto& a = std::get<IR_Assign>(instrs[i]);
            if (!used.count(a.dest)) { ++i; continue; }
        }
        else if (std::holds_alternative<IR_BinOp>(instrs[i])) {
            const auto& b = std::get<IR_BinOp>(instrs[i]);
            if (!used.count(b.dest)) { ++i; continue; }
        }
        else if (std::holds_alternative<IR_Call>(instrs[i])) {
            const auto& c = std::get<IR_Call>(instrs[i]);
            if (!c.dest.empty() && !used.count(c.dest)) { ++i; continue; }
        }

        if (std::holds_alternative<IR_CondJump>(instrs[i])) {
            i = emitIfElse(instrs, i, out, indentLevel, used);
        } else {
            emitInstruction(instrs[i], out, indentLevel);
            ++i;
        }
    }
}

size_t CGenerator::emitIfElse(const std::vector<IRInstruction>& instrs, size_t i, std::ostream& out, int indentLevel, const std::unordered_set<std::string>& used) {
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
            i = emitIfElse(instrs, i, out, indentLevel + 1, used);
        } else if (std::holds_alternative<IR_Assign>(instrs[i])) {
            const auto& a = std::get<IR_Assign>(instrs[i]);
            if (!a.src.empty()) emitInstruction(instrs[i], out, indentLevel + 1);
            ++i;
        } else if (std::holds_alternative<IR_Assign>(instrs[i])) {
            const auto& a = std::get<IR_Assign>(instrs[i]);
            if (used.count(a.dest)) emitInstruction(instrs[i], out, indentLevel + 1);
            ++i;
        } else if (std::holds_alternative<IR_BinOp>(instrs[i])) {
            const auto& b = std::get<IR_BinOp>(instrs[i]);
            if (used.count(b.dest)) emitInstruction(instrs[i], out, indentLevel + 1);
            ++i;
        } else if (std::holds_alternative<IR_Call>(instrs[i])) {
            const auto& c = std::get<IR_Call>(instrs[i]);
            if (c.dest.empty() || used.count(c.dest)) emitInstruction(instrs[i], out, indentLevel + 1);
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
                    i = emitIfElse(instrs, i, out, indentLevel + 1, used);
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

std::vector<std::pair<IRType, std::string>> CGenerator::collectDecls(const IR_Block& block, const std::unordered_set<std::string>& used) {

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
}

void CGenerator::emitInstruction(const IRInstruction& instr, std::ostream& out, int indentLevel) {

    // --- IR_Assign : dest = src; ---
    if (std::holds_alternative<IR_Assign>(instr)) {
        const auto& a = std::get<IR_Assign>(instr);
        out << indent(indentLevel) << a.dest << " = ";

        if (a.src == "NULL") {
            out << "0";
        } else if (a.type == IRType::STRING && isLiteral(a.src)) {
            out << "ENCODE_STR(" << a.src << ")";  // ← string dans lisp_obj
        } else if ((a.type == IRType::INT || a.type == IRType::UNKNOWN)
                   && isLiteral(a.src) && a.src != "0") {
            out << "ENCODE_INT(" << a.src << ")";
        } else {
            out << a.src;
        }
        out << ";\n";
        return;
    }


    if (std::holds_alternative<IR_BinOp>(instr)) {
        const auto& b = std::get<IR_BinOp>(instr);

        bool isComparison = (b.op == "==" || b.op == "<" || b.op == ">" ||
                             b.op == "<=" || b.op == ">=");

        std::string left  = isLiteral(b.left)  ? "ENCODE_INT(" + b.left + ")"  : b.left;
        std::string right = isLiteral(b.right) ? "ENCODE_INT(" + b.right + ")" : b.right;

        out << indent(indentLevel);
        if (isComparison) {
            out << b.dest << " = DECODE_INT(" << left << ") "
                << b.op << " DECODE_INT(" << right << ");\n";
        } else {
            out << b.dest << " = ENCODE_INT(DECODE_INT(" << left << ") "
                << b.op << " DECODE_INT(" << right << "));\n";
        }
        return;
    }
    // --- IR_Call : dest = funcName(arg1, arg2, ...); ---
    //              ou funcName(arg1, arg2, ...);  si dest est vide (void)
    if (std::holds_alternative<IR_Call>(instr)) {
        const auto& c = std::get<IR_Call>(instr);
        out << indent(indentLevel);
        if (!c.dest.empty()) out << c.dest << " = ";
        out << c.funcName << "(";
        for (size_t i = 0; i < c.args.size(); ++i) {
            std::string arg = c.args[i];
            if (arg == "NULL") {
                out << "0";
            } else if (isLiteral(arg) && !arg.empty() && arg[0] == '"') {
                out << "ENCODE_STR(" << arg << ")";  // ← au lieu de (lisp_obj)
            } else if (isLiteral(arg) && arg != "0") {
                out << "ENCODE_INT(" << arg << ")";
            } else {
                out << arg;
            }
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
    if (std::holds_alternative<IR_Label>(instr)) {
        const auto& l = std::get<IR_Label>(instr);
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

    // --- IR_Print ---
    if (auto* p = std::get_if<IR_Print>(&instr)) {
        out << indent(indentLevel);
        if (!p->value.empty() && p->value[0] == '"') {
            out << "printf(\"%s\\n\", " << p->value << ");\n";
        } else if (p->type == IRType::STRING) {
            out << "printf(\"%s\\n\", " << p->value << ");\n";
        } else if (p->type == IRType::FLOAT) {
            out << "printf(\"%f\\n\", " << p->value << ");\n";
        } else if (p->type == IRType::STRING) {
            // Vérifie si c'est un lisp_obj encodé ou une char* directe
            out << "if (IS_STR(" << p->value << ")) "
                << "printf(\"%s\\n\", DECODE_STR(" << p->value << ")); "
                << "else printf(\"%ld\\n\", (long)DECODE_INT(" << p->value << "));\n";
        } else {
            out << "printf(\"%ld\\n\", (long)DECODE_INT(" << p->value << "));\n";
        }
        return;
    }

    // --- IR_Scan : scanf(format, &dest); ---
    if (std::holds_alternative<IR_Scan>(instr)) {
        const auto& s = std::get<IR_Scan>(instr);
        out << indent(indentLevel) << "scanf(\"" << printfFormat(s.type) << "\", ";
        if (s.type == IRType::STRING) {
            out << s.dest;
        } else {
            out << "&" << s.dest;
        }
        out << ");\n";
        return;
    }

    // --- IR_FuncDecl inline : ne devrait pas apparaître dans un bloc ---
    if (std::holds_alternative<IR_FuncDecl>(instr)) {
        out << indent(indentLevel) << "/* [IR_FuncDecl inline — ignoré] */\n";
        return;
    }

    out << indent(indentLevel) << "/* [instruction non gérée explicitement] : ";
    std::visit([&out](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, IR_Assign>) out << "IR_Assign";
        else if constexpr (std::is_same_v<T, IR_BinOp>) out << "IR_BinOp";
        else if constexpr (std::is_same_v<T, IR_Call>) out << "IR_Call";
        else if constexpr (std::is_same_v<T, IR_FuncDecl>) out << "IR_FuncDecl";
        else if constexpr (std::is_same_v<T, IR_CondJump>) out << "IR_CondJump";
        else if constexpr (std::is_same_v<T, IR_Label>) out << "IR_Label";
        else if constexpr (std::is_same_v<T, IR_Jump>) out << "IR_Jump";
        else if constexpr (std::is_same_v<T, IR_Return>) out << "IR_Return";
        else if constexpr (std::is_same_v<T, IR_Print>) out << "IR_Print";
        else if constexpr (std::is_same_v<T, IR_Scan>) out << "IR_Scan";
        else out << "Unknown Type";
    }, instr);
    out << " */\n";
}

// =============================================================================
// Utilitaires
// =============================================================================

std::string CGenerator::indent(int level) {
    return std::string(level * 4, ' '); // 4 espaces par niveau
}

std::string CGenerator::printfFormat(IRType type) {
    switch (type) {
        case IRType::INT:    return "%ld"; // lisp_obj est souvent un long
        case IRType::BOOL:   return "%ld";
        case IRType::LIST:   return "%ld";
        case IRType::STRING: return "%s";
        case IRType::FLOAT:  return "%f";
        default:             return "%ld";
    }
}

bool CGenerator::isTemp(const std::string& name) {
    // Un temporaire commence par 't' suivi uniquement de chiffres : t0, t12, ...
    if (name.empty() || name[0] != 't') return false;
    return std::all_of(name.begin() + 1, name.end(), ::isdigit);
}

bool CGenerator::isLiteral(const std::string& name) {
    if (name.empty() || name == "NULL") return true; // Commence par un chiffre → constante numérique : 42, 3.14f
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
        case IRType::INT:
        case IRType::LIST:
        case IRType::BOOL:
            // L'entier, la liste et le booléen Lisp partagent le même conteneur.
            // Le type 'lisp_obj' est défini dans ton runtime comme un intptr_t.
            return "lisp_obj";

        case IRType::FLOAT:
            return "float";

        case IRType::STRING:
            return "char*";

        case IRType::VOID:
            return "void";

        default:
            // Par sécurité, on traite tout ce qui est inconnu comme un objet Lisp.
            return "lisp_obj";
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

std::unordered_set<std::string> CGenerator::computeUsed(const IR_Block& block) {
    std::unordered_set<std::string> used;

    // Passe 1 : collecte uniquement les "racines" — variables consommées
    // par des instructions sans dest (print, condJump, return, call void)
    for (const auto& instr : block.instructions) {
        if (std::holds_alternative<IR_Print>(instr)) {
            const auto& p = std::get<IR_Print>(instr);
            if (!isLiteral(p.value)) used.insert(p.value);
        }
        else if (std::holds_alternative<IR_CondJump>(instr)) {
            const auto& j = std::get<IR_CondJump>(instr);
            if (!isLiteral(j.condition)) used.insert(j.condition);
        }
        else if (std::holds_alternative<IR_Return>(instr)) {
            const auto& r = std::get<IR_Return>(instr);
            if (!r.value.empty() && !isLiteral(r.value)) used.insert(r.value);
        }
        else if (std::holds_alternative<IR_Call>(instr)) {
            const auto& c = std::get<IR_Call>(instr);
            // Call void (pas de dest) → ses args sont racines
            if (c.dest.empty()) {
                for (const auto& arg : c.args)
                    if (!isLiteral(arg)) used.insert(arg);
            }
        }
        else if (std::holds_alternative<IR_Scan>(instr)) {
            const auto& s = std::get<IR_Scan>(instr);
            if (!isLiteral(s.dest)) used.insert(s.dest);
        }
    }

    // Passe 2 : propagation en cascade
    // Si dest est dans used → ses sources le deviennent aussi
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& instr : block.instructions) {
            if (std::holds_alternative<IR_BinOp>(instr)) {
                const auto& b = std::get<IR_BinOp>(instr);
                if (used.count(b.dest)) {
                    if (!isLiteral(b.left) && !used.count(b.left))   { used.insert(b.left);  changed = true; }
                    if (!isLiteral(b.right) && !used.count(b.right)) { used.insert(b.right); changed = true; }
                }
            }
            else if (std::holds_alternative<IR_Assign>(instr)) {
                const auto& a = std::get<IR_Assign>(instr);
                if (used.count(a.dest) && !isLiteral(a.src) && !used.count(a.src)) {
                    used.insert(a.src);
                    changed = true;
                }
            }
            else if (std::holds_alternative<IR_Call>(instr)) {
                const auto& c = std::get<IR_Call>(instr);
                if (!c.dest.empty() && used.count(c.dest)) {
                    for (const auto& arg : c.args) {
                        if (!isLiteral(arg) && !used.count(arg)) {
                            used.insert(arg);
                            changed = true;
                        }
                    }
                }
            }
        }
        for (const auto& [decl, body] : program_.functions) {
            for (const auto& instr : body.instructions) {
                if (std::holds_alternative<IR_BinOp>(instr)) {
                    const auto& b = std::get<IR_BinOp>(instr);
                    if (!isLiteral(b.left) && !isTemp(b.left))   used.insert(b.left);
                    if (!isLiteral(b.right) && !isTemp(b.right)) used.insert(b.right);
                }
                else if (std::holds_alternative<IR_Assign>(instr)) {
                    const auto& a = std::get<IR_Assign>(instr);
                    if (!isLiteral(a.src) && !isTemp(a.src)) used.insert(a.src);
                }
            }
        }
    }

    return used;
}