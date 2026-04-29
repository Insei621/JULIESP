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

int main(void) {
    int mon_age;
    int t3;

    printf("%s\n", "--- Test du Load ---");
    mon_age = 20;
    if (0) {
        printf("%s\n", "Vous êtes majeur.");
        printf("%s\n", "Le carré de votre âge est :");
        t3 = carre(mon_age);
        printf("%d\n", t3);
    } else {
        printf("%s\n", "Vous êtes mineur.");
    }
    return 0;
}
