//
// Created by kerya on 16/04/2026.
//

#include "../include/AST.h"
#include "../include/Visitor.h"

void IntegerLit::accept(Visitor* v) { v->visit(this); }
void FloatLit::accept(Visitor* v)   { v->visit(this); }
void CharLit::accept(Visitor* v)    { v->visit(this); }
void StringLit::accept(Visitor* v)  { v->visit(this); }
void BoolLit::accept(Visitor* v)    { v->visit(this); }
void Identifier::accept(Visitor* v) { v->visit(this); }
void Primitive::accept(Visitor* v)  { v->visit(this); }
void SExpr::accept(Visitor* v)      { v->visit(this); }