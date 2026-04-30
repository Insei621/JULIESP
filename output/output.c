/* Code généré automatiquement — ne pas éditer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lisp_runtime.h"

int afficher_infos(char* nom, int age) {
    printf("%s\n", "Nom :");
    printf("%d\n", nom);
    printf("%s\n", "Age :");
    printf("%d\n", age);
}

int main(void) {
    int score;
    int t0;

    printf("%s\n", "Etape 1");
    printf("%s\n", "Etape 2");
    printf("%s\n", "Etape 3");
    score = 75;
    t0 = score > 50;
    if (t0) {
        printf("%s\n", "Score suffisant :");
        printf("%d\n", score);
        printf("%s\n", "Vous avez reussi.");
    } else {
        printf("%s\n", "Score insuffisant.");
    }
    t2 = afficher_infos("Alice", 30);
    t3 = afficher_infos("Bob", 25);
    return 0;
}
