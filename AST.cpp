//
// Created by kerya on 15/04/2026.
//

#include "AST.h"
#include "pch.h"

class ASTNode {
public:
    // Le constructeur force maintenant à définir si le nœud est quoté
    ASTNode(std::string n, int l, int c, bool quoted = false)
        : name(std::move(n)), line(l), col(c), isQuoted(quoted) {}
    virtual ~ASTNode() = default;

    // Une petite méthode utilitaire pour le futur évaluateur
    bool isQuotedNode() const { return isQuoted; }

protected:
    std::string name;
    int line;
    int col;
    bool isQuoted; // Tout le monde possède cette propriété maintenant !
};

// --- LES ATOMES ---
class Atom : public ASTNode {
public:
    using ASTNode::ASTNode; // Utilise le constructeur complet d'ASTNode
};

// --- LES SYMBOLES ---
class Symbol : public Atom { public: using Atom::Atom; };
class Primitive : public Symbol { public: using Symbol::Symbol; };
class Identifier : public Symbol { public: using Symbol::Symbol; };

// --- LES LITTÉRAUX ---
class Literal : public Atom { public: using Atom::Atom; };

class IntegerLit : public Literal {
public:
    int value;
    IntegerLit(int v, int l, int c, bool quoted = false)
        : Literal("INT", l, c, quoted), value(v) {}
};

class FloatLit : public Literal {
public:
    float value;
    FloatLit(float v, int l, int c, bool quoted = false)
        : Literal("FLOAT", l, c, quoted), value(v) {}
};

class CharLit : public Literal {
public:
    char value;
    CharLit(char v, int l, int c, bool quoted = false)
        : Literal("CHAR", l, c, quoted), value(v) {}
};

class StringLit : public Literal {
public:
    std::string value;
    StringLit(std::string v, int l, int c, bool quoted = false)
        : Literal("STRING", l, c, quoted), value(std::move(v)) {}
};

class BoolLit : public Literal {
public:
    bool value;
    BoolLit(bool v, int l, int c, bool quoted = false)
        : Literal("BOOL", l, c, quoted), value(v) {}
};

// --- LES S-EXPRESSIONS ---
class SExpr : public ASTNode {
public:
    // Le constructeur appelle ASTNode, qui gère déjà le isQuoted
    SExpr(int l, int c, bool quoted = false)
        : ASTNode("SExpr", l, c, quoted) {}

    void add(ASTNode* node) { children.push_back(node); }

protected:
    std::vector<ASTNode*> children;
    // Note : isQuoted a été supprimé d'ici car il est hérité du parent.
};