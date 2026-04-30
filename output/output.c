/* Code généré automatiquement — ne pas éditer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lisp_runtime.h"

int premier(int lst) {
    int t0;

    t0 = lisp_<<(lst);
    return t0;
}

int deuxieme(int lst) {
    int t1;
    int t2;

    t1 = lisp_>>(lst);
    t2 = lisp_<<(t1);
    return t2;
}

int troisieme(int lst) {
    int t3;
    int t4;
    int t5;

    t3 = lisp_>>(lst);
    t4 = lisp_>>(t3);
    t5 = lisp_<<(t4);
    return t5;
}

int fst(int paire) {
    int t6;

    t6 = lisp_<<(paire);
    return t6;
}

int snd(int paire) {
    int t7;
    int t8;

    t7 = lisp_>>(paire);
    t8 = lisp_<<(t7);
    return t8;
}

void describe(int x) {
    int t9;

    t9 = lisp_@(x);
    if (t9) {
        printf("%s\n", "C est un atome");
    } else {
        printf("%s\n", "C est une liste");
    }
}

void est_vide(int lst) {
    int t10;

    t10 = lisp_|(lst);
    if (t10) {
        printf("%s\n", "Liste vide");
    } else {
        printf("%s\n", "Liste non vide");
    }
}

int main(void) {
    int t11;
    int t12;
    int t13;
    int nums;
    int t14;
    int t15;
    int t16;
    int t17;
    int p;
    int t18;
    int t19;
    int a;
    int b;
    int c;
    int t20;
    int t21;
    int t22;
    int liste3;
    int t23;
    int t24;
    int t25;

    t11 = lisp_&(30, );
    t12 = lisp_&(20, t11);
    t13 = lisp_&(10, t12);
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
    t17 = lisp_&(100, 200);
    p = t17;
    printf("%s\n", "fst(p) =");
    t18 = fst(p);
    printf("%d\n", t18);
    printf("%s\n", "snd(p) =");
    t19 = snd(p);
    printf("%d\n", t19);
    printf("%s\n", "=== Atom? et null? dans fonctions ===");
    describe(42);
    describe(nums);
    est_vide();
    est_vide(nums);
    printf("%s\n", "=== Construction et déconstruction ===");
    a = 1;
    b = 2;
    c = 3;
    t20 = lisp_&(c, );
    t21 = lisp_&(b, t20);
    t22 = lisp_&(a, t21);
    liste3 = t22;
    printf("%s\n", "Liste (a b c), tete :");
    t23 = lisp_<<(liste3);
    printf("%d\n", t23);
    printf("%s\n", "Queue de la liste, tete :");
    t24 = lisp_>>(liste3);
    t25 = lisp_<<(t24);
    printf("%d\n", t25);
    return 0;
}
