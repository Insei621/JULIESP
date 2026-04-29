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

int est_majeur(int age) {
    int t1;

    t1 = age > 17;
    return t1;
}

int main(void) {
    int mon_age;
    int t2;
    int t4;

    printf("%s\n", "--- Test du Load ---");
    mon_age = 20;
    t2 = est_majeur(mon_age);
    if (t2) {
        printf("%s\n", "Vous êtes majeur.");
        printf("%s\n", "Le carré de votre âge est :");
        t4 = carre(mon_age);
        printf("%d\n", t4);
    } else {
        printf("%s\n", "Vous êtes mineur.");
    }
    return 0;
}
