/* Code généré automatiquement — ne pas éditer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lisp_runtime.h"

int main(void) {
    int x;
    int t0;
    char* a;
    char* b;
    int t2;

    x = 10;
    t0 = x > 5;
    if (t0) {
        printf("%s\n", "X est grand");
    } else {
        printf("%s\n", "X est petit");
    }
    a = "hello";
    b = a;
    t2 = a == b;
    printf("%d\n", t2);
    return 0;
}
