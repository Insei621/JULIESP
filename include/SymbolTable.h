#ifndef COMPILATEUR_JULIESP_SYMBOLTABLE_H
#define COMPILATEUR_JULIESP_SYMBOLTABLE_H

#include "pch.h"
#include "AST.h"
#include <vector> // Nécessaire pour stocker les enfants

class Scope {
private:
    std::unordered_map<std::string, ASTNode*> symbols;
    Scope* enclosing_scope;
    std::vector<Scope*> children;

public:
    Scope(Scope* parent = nullptr);

    // On ajoute une méthode pour enregistrer un enfant
    void addChild(Scope* child) { children.push_back(child); }
    const std::vector<Scope*>& getChildren() const { return children; }

    void enter(const std::string& name, ASTNode* node);
    ASTNode* lookup(const std::string& name);

    Scope* getEnclosingScope() const;
    const std::unordered_map<std::string, ASTNode*>& getSymbols() const;
};

class Symtable {
private:
    Scope* current_scope;
    Scope* global_scope; // <-- AJOUT : Pour toujours avoir la racine de l'arbre

public:
    Symtable();
    ~Symtable(); // Très important ici pour nettoyer tout l'arbre

    void openScope();
    void closeScope();

    void enter(const std::string& name, ASTNode* node);
    ASTNode* lookup(const std::string& name);

    Scope* getCurrentScope() const;
    Scope* getGlobalScope() const { return global_scope; } // <-- AJOUT
};

#endif //COMPILATEUR_JULIESP_SYMBOLTABLE_H