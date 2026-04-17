#include "../include/SymbolTable.h"

// --- SCOPE ---
Scope::Scope(Scope* parent) : enclosing_scope(parent) {}

void Scope::enter(const std::string& name, ASTNode* node) {
    symbols[name] = node;
}

ASTNode* Scope::lookup(const std::string& name) {
    // 1. On cherche dans le scope actuel
    if (symbols.find(name) != symbols.end()) {
        return symbols[name];
    }
    // 2. Si pas trouvé, on remonte récursivement chez le parent
    if (enclosing_scope) {
        return enclosing_scope->lookup(name);
    }
    return nullptr;
}

Scope* Scope::getEnclosingScope() const {
    return enclosing_scope;
}

const std::unordered_map<std::string, ASTNode*>& Scope::getSymbols() const {
    return symbols;
}

// --- SYMTABLE ---
Symtable::Symtable() {
    global_scope = new Scope(nullptr);
    current_scope = new Scope(); // Scope Global par défaut
}

// Le destructeur (Optionnel mais propre)
Symtable::~Symtable() {
    // Note : Dans un vrai compilateur on viderait la pile ici
}

void Symtable::openScope() {
    Scope* newScope = new Scope(current_scope);

    // Si on n'est pas à la racine, on dit au parent qu'il a un nouvel enfant
    if (current_scope) {
        current_scope->addChild(newScope);
    }

    current_scope = newScope;
}

void Symtable::closeScope() {
    if (current_scope->getEnclosingScope()) {
        current_scope = current_scope->getEnclosingScope();
        // IMPORTANT : On ne fait PAS de "delete".
        // On remonte juste le pointeur pour garder les données en mémoire.
    }
}

void Symtable::enter(const std::string& name, ASTNode* node) {
    current_scope->enter(name, node);
}

ASTNode* Symtable::lookup(const std::string& name) {
    return current_scope->lookup(name);
}

Scope* Symtable::getCurrentScope() const {
    return current_scope;
}