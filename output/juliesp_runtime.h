#ifndef JULIESP_RUNTIME_H
#define JULIESP_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef intptr_t lisp_obj;

typedef struct Node {
    lisp_obj value;
    struct Node* next;
} Node;

/* --- Tags --- */
#define TAG_PTR 0   // 00 → Node*
#define TAG_INT 1   // 01 → entier
#define TAG_STR 2   // 10 → char*

#define GET_TAG(x)      ((x) & 3)
#define IS_INT(x)       (GET_TAG(x) == TAG_INT)
#define IS_PTR(x)       (GET_TAG(x) == TAG_PTR)
#define IS_STR(x)       (GET_TAG(x) == TAG_STR)
#define IS_NIL(x)       ((x) == 0)

#define ENCODE_INT(n)   ((lisp_obj)(((intptr_t)(n) << 2) | TAG_INT))
#define DECODE_INT(x)   ((int)((x) >> 2))

#define ENCODE_STR(s)   ((lisp_obj)((intptr_t)(s) | TAG_STR))
#define DECODE_STR(x)   ((char*)((x) & ~(intptr_t)3))

/* --- Primitives --- */
static lisp_obj lisp_cons(lisp_obj val, lisp_obj next_obj) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->value = val;
    n->next  = (Node*)(next_obj & ~(intptr_t)3);
    return (lisp_obj)n;
}

static lisp_obj lisp_car(lisp_obj lst) {
    if (IS_NIL(lst)) { fprintf(stderr, "car: liste vide\n"); exit(1); }
    if (!IS_PTR(lst)) { fprintf(stderr, "car: attend une liste\n"); exit(1); }
    return ((Node*)lst)->value;
}

static lisp_obj lisp_cdr(lisp_obj lst) {
    if (IS_NIL(lst)) { fprintf(stderr, "cdr: liste vide\n"); exit(1); }
    if (!IS_PTR(lst)) { fprintf(stderr, "cdr: attend une liste\n"); exit(1); }
    return (lisp_obj)((Node*)lst)->next;
}

static int lisp_null(lisp_obj lst) {
    return IS_NIL(lst);
}

static int lisp_atom(lisp_obj x) {
    return IS_NIL(x) || IS_INT(x) || IS_STR(x);
}

static int lisp_numberp(lisp_obj x) {
    return IS_INT(x);
}

/* --- Affichage d'un lisp_obj --- */
static void lisp_print_obj(lisp_obj x) {
    if (IS_NIL(x))       printf("nil");
    else if (IS_INT(x))  printf("%d", DECODE_INT(x));
    else if (IS_STR(x))  printf("%s", DECODE_STR(x));
    else                 printf("<list>");
}

#endif