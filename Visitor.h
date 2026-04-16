//
// Created by kerya on 16/04/2026.
//

#ifndef COMPILATEUR_JULIESP_VISITOR_H
#define COMPILATEUR_JULIESP_VISITOR_H

#include "pch.h"

// 1. Forward declarations HORS de la classe
class SExpr;
class Identifier;
class Primitive;
class IntegerLit;
class FloatLit;
class StringLit;
class BoolLit;
class CharLit;

class Visitor {
public:
    virtual ~Visitor() = default;

    // 2. Utilisation simple des types
    virtual void visit(SExpr* node) = 0;
    virtual void visit(Identifier* node) = 0;
    virtual void visit(Primitive* node) = 0;
    virtual void visit(IntegerLit* node) = 0;
    virtual void visit(FloatLit* node) = 0;
    virtual void visit(StringLit* node) = 0;
    virtual void visit(BoolLit* node) = 0;
    virtual void visit(CharLit* node) = 0;
};


#endif //COMPILATEUR_JULIESP_VISITOR_H