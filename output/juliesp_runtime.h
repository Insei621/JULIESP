#ifndef JULIESP_RUNTIME_H
#define JULIESP_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>

/* --- Type liste chaînée --- */
typedef struct Node {
    int value;
    struct Node* next;
} Node;

/* --- Primitives --- */
static Node* lisp_cons(int val, Node* next) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->value = val;
    n->next  = next;
    return n;
}

static int lisp_car(Node* lst) {
    if (!lst) { fprintf(stderr, "car: liste vide\n"); exit(1); }
    return lst->value;
}

static Node* lisp_cdr(Node* lst) {
    if (!lst) { fprintf(stderr, "cdr: liste vide\n"); exit(1); }
    return lst->next;
}

static int lisp_null(Node* lst) { return lst == NULL; }
static int lisp_atom_int (int   x) { (void)x; return 1; }  /* int → toujours atome */
static int lisp_atom_list(Node* x) { (void)x; return 0; }  /* liste → jamais atome */
static int lisp_numberp(Node* x) {
    (void)x;
    return 1;
}

#endif