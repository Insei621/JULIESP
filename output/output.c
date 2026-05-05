/* Code généré automatiquement — ne pas éditer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./juliesp_runtime.h"

/* --- Prototypes --- */
int premier(Node* lst);
int deuxieme(Node* lst);
int troisieme(Node* lst);
int fst(Node* paire);
int snd(Node* paire);
void describe(Node* x);
void est_vide(Node* lst);

int premier(Node* lst) {
    int t0;

    t0 = lisp_car(lst);
    return t0;
}

int deuxieme(Node* lst) {
    Node* t1;
    int t2;

    t1 = lisp_cdr(lst);
    t2 = lisp_car(t1);
    return t2;
}

int troisieme(Node* lst) {
    Node* t3;
    Node* t4;
    int t5;

    t3 = lisp_cdr(lst);
    t4 = lisp_cdr(t3);
    t5 = lisp_car(t4);
    return t5;
}

int fst(Node* paire) {
    int t6;

    t6 = lisp_car(paire);
    return t6;
}

int snd(Node* paire) {
    Node* t7;
    int t8;

    t7 = lisp_cdr(paire);
    t8 = lisp_car(t7);
    return t8;
}

void describe(Node* x) {
    int t9;

    t9 = lisp_atom_int(x);
    if (t9) {
        printf("%s\n", "C est un atome");
    } else {
        printf("%s\n", "C est une liste");
    }
}

void est_vide(Node* lst) {
    int t10;

    t10 = lisp_null(lst);
    if (t10) {
        printf("%s\n", "Liste vide");
    } else {
        printf("%s\n", "Liste non vide");
    }
}

int main(void) {
    Node* t11;
    Node* t12;
    Node* t13;
    Node* nums;
    int t14;
    int t15;
    int t16;
    Node* t17;
    Node* t18;
    Node* p;
    int t19;
    int t20;
    int a;
    int b;
    int c;
    Node* t21;
    Node* t22;
    Node* t23;
    Node* liste3;
    int t24;
    Node* t25;
    int t26;

    t11 = lisp_cons(30, NULL);
    t12 = lisp_cons(20, t11);
    t13 = lisp_cons(10, t12);
    nums = t13;
    printf("%s\n", "=== Accès par position ===");
    printf("%s\n", "Element 1 :");
    t14 = premier(nums);
    printf("%d\n", t14);
    printf("%s\n", "Element 2 :");
    t15 = deuxieme(nums);
    printf("%d\n", t15);
    printf("%s\n", "Element 3 :");
    t16 = troisieme(nums);
    printf("%d\n", t16);
    printf("%s\n", "=== Test paires ===");
    t17 = lisp_cons(200, NULL);
    t18 = lisp_cons(100, t17);
    p = t18;
    printf("%s\n", "fst(p) =");
    t19 = fst(p);
    printf("%d\n", t19);
    printf("%s\n", "snd(p) =");
    t20 = snd(p);
    printf("%d\n", t20);
    printf("%s\n", "=== Atom? et null? dans fonctions ===");
    describe(NULL);
    describe(nums);
    est_vide(NULL);
    est_vide(nums);
    printf("%s\n", "=== Construction et déconstruction ===");
    a = 1;
    b = 2;
    c = 3;
    t21 = lisp_cons(c, NULL);
    t22 = lisp_cons(b, t21);
    t23 = lisp_cons(a, t22);
    liste3 = t23;
    printf("%s\n", "Liste (a b c), tete :");
    t24 = lisp_car(liste3);
    printf("%d\n", t24);
    printf("%s\n", "Queue de la liste, tete :");
    t25 = lisp_cdr(liste3);
    t26 = lisp_car(t25);
    printf("%d\n", t26);
    return 0;
}
