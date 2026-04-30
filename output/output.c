/* Code généré automatiquement — ne pas éditer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lisp_runtime.h"

int carre(int n) {
    int t0;

    t0 = n * n;
    return t0;
}

int cube(int n) {
    int t1;
    int t2;

    t1 = n * n;
    t2 = n * t1;
    return t2;
}

int abs_val(int n) {
    int t3;
    int t4;
    int t5;

    t3 = n > 0;
    if (t3) {
        t4 = n;
    } else {
        t5 = 0 - n;
        t4 = t5;
    }
    return t4;
}

int est_positif(int n) {
    int t6;

    t6 = n > 0;
    return t6;
}

int main(void) {
    int t7;
    int t8;
    int t9;
    int t10;
    int t11;

    printf("%s\n", "=== Test du Load ===");
    printf("%s\n", "carre(6) =");
    t7 = carre(6);
    printf("%d\n", t7);
    printf("%s\n", "cube(3) =");
    t8 = cube(3);
    printf("%d\n", t8);
    printf("%s\n", "abs_val(-5) =");
    t9 = abs_val(-5);
    printf("%d\n", t9);
    printf("%s\n", "est_positif(10) =");
    t10 = est_positif(10);
    printf("%d\n", t10);
    printf("%s\n", "est_positif(-3) =");
    t11 = est_positif(-3);
    printf("%d\n", t11);
    return 0;
}
