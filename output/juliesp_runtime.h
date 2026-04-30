// juliesp_runtime.h — Runtime minimal pour les listes JuliesSP

#ifndef JULIESP_RUNTIME_H
#define JULIESP_RUNTIME_H

#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
    int value;
    struct Node* next;
} Node;

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

static int lisp_null(Node* lst) {
    return lst == NULL;
}

static int lisp_atom(int x) {
    (void)x;
    return 1;
}

#endif