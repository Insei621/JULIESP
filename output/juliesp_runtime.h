#ifndef JULIESP_RUNTIME_H
#define JULIESP_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* --- Type universel Lisp --- */
// intptr_t garantit qu'on peut stocker soit un int, soit un pointeur.
typedef intptr_t lisp_obj;

typedef struct Node {
    lisp_obj value;      // CAR : peut être un int tagué ou un pointeur
    struct Node* next;   // CDR : pointeur vers le prochain Node
} Node;

/* --- Macros de gestion du Tagging --- */
// Si le bit de poids faible est 1 -> c'est un entier. Sinon -> pointeur.
#define IS_INT(x)      ((x) & 1)
#define IS_PTR(x)      (!((x) & 1))
#define ENCODE_INT(n)  ((lisp_obj)(((intptr_t)(n) << 1) | 1))
#define DECODE_INT(x)  ((int)((x) >> 1))

/* --- Primitives --- */

// CONS : prend deux lisp_obj, crée un Node, retourne l'adresse (bit de fin = 0)
static lisp_obj lisp_cons(lisp_obj val, lisp_obj next_obj) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->value = val;
    n->next  = (Node*)next_obj; // On cast l'objet vers le pointeur réel
    return (lisp_obj)n;         // L'adresse d'un malloc est alignée (finit par 0)
}

// CAR : accède à la valeur du Node
static lisp_obj lisp_car(lisp_obj lst) {
    if (lst == 0) { fprintf(stderr, "car: liste vide\n"); exit(1); }
    if (IS_INT(lst)) { fprintf(stderr, "car: attend une liste, reçu un entier\n"); exit(1); }
    return ((Node*)lst)->value;
}

// CDR : accède au pointeur suivant
static lisp_obj lisp_cdr(lisp_obj lst) {
    if (lst == 0) { fprintf(stderr, "cdr: liste vide\n"); exit(1); }
    if (IS_INT(lst)) { fprintf(stderr, "cdr: attend une liste, reçu un entier\n"); exit(1); }
    return (lisp_obj)((Node*)lst)->next;
}

// NULL : vrai si c'est le pointeur 0
static __attribute__((unused)) int lisp_null(lisp_obj lst) {
    return lst == 0;
}

// ATOM : vrai si c'est NULL ou si c'est un entier tagué
static __attribute__((unused)) int lisp_atom(lisp_obj x) {
    return (x == 0) || IS_INT(x);
}

// NUMBERP : vrai uniquement si c'est un entier tagué
static __attribute__((unused)) int lisp_numberp(lisp_obj x) {
    return IS_INT(x);
}

#endif