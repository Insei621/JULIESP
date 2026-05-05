/* Code généré automatiquement — ne pas éditer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./juliesp_runtime.h"

/* --- Prototypes --- */
lisp_obj premier(lisp_obj lst);
lisp_obj deuxieme(lisp_obj lst);
lisp_obj troisieme(lisp_obj lst);
lisp_obj fst(lisp_obj paire);
lisp_obj snd(lisp_obj paire);
lisp_obj describe(lisp_obj x);
lisp_obj est_vide(lisp_obj lst);

lisp_obj premier(lisp_obj lst) {
    lisp_obj t0;

    t0 = lisp_car(lst);
    return t0;
}

lisp_obj deuxieme(lisp_obj lst) {
    lisp_obj t1;
    lisp_obj t2;

    t1 = lisp_cdr(lst);
    t2 = lisp_car(t1);
    return t2;
}

lisp_obj troisieme(lisp_obj lst) {
    lisp_obj t3;
    lisp_obj t4;
    lisp_obj t5;

    t3 = lisp_cdr(lst);
    t4 = lisp_cdr(t3);
    t5 = lisp_car(t4);
    return t5;
}

lisp_obj fst(lisp_obj paire) {
    lisp_obj t6;

    t6 = lisp_car(paire);
    return t6;
}

lisp_obj snd(lisp_obj paire) {
    lisp_obj t7;
    lisp_obj t8;

    t7 = lisp_cdr(paire);
    t8 = lisp_car(t7);
    return t8;
}

lisp_obj describe(lisp_obj x) {
    lisp_obj t9;

    t9 = lisp_atom(x);
    if (t9) {
        printf("%s\n", "C est un atome");
    } else {
        printf("%s\n", "C est une liste");
    }
    return 0; // NIL par défaut
}

lisp_obj est_vide(lisp_obj lst) {
    lisp_obj t10;

    t10 = lisp_null(lst);
    if (t10) {
        printf("%s\n", "Liste vide");
    } else {
        printf("%s\n", "Liste non vide");
    }
    return 0; // NIL par défaut
}

int main(void) {
    lisp_obj t11;
    lisp_obj t12;
    lisp_obj t13;
    lisp_obj nums;
    lisp_obj t14;
    lisp_obj t15;
    lisp_obj t16;
    lisp_obj t17;
    lisp_obj t18;
    lisp_obj p;
    lisp_obj t19;
    lisp_obj t20;
    lisp_obj a;
    lisp_obj b;
    lisp_obj c;
    lisp_obj t21;
    lisp_obj t22;
    lisp_obj t23;
    lisp_obj liste3;
    lisp_obj t24;
    lisp_obj t25;
    lisp_obj t26;

    t11 = lisp_cons(ENCODE_INT(30), 0);
    t12 = lisp_cons(ENCODE_INT(20), t11);
    t13 = lisp_cons(ENCODE_INT(10), t12);
    nums = t13;
    /* [instruction non gérée explicitement] : IR_Assign */
    printf("%s\n", "=== Accès par position ===");
    printf("%s\n", "Element 1 :");
    t14 = premier(nums);
    printf("%ld\n", (long)DECODE_INT(t14));
    printf("%s\n", "Element 2 :");
    t15 = deuxieme(nums);
    printf("%ld\n", (long)DECODE_INT(t15));
    printf("%s\n", "Element 3 :");
    t16 = troisieme(nums);
    printf("%ld\n", (long)DECODE_INT(t16));
    printf("%s\n", "=== Test paires ===");
    t17 = lisp_cons(ENCODE_INT(200), 0);
    t18 = lisp_cons(ENCODE_INT(100), t17);
    p = t18;
    /* [instruction non gérée explicitement] : IR_Assign */
    printf("%s\n", "fst(p) =");
    t19 = fst(p);
    printf("%ld\n", (long)DECODE_INT(t19));
    printf("%s\n", "snd(p) =");
    t20 = snd(p);
    printf("%ld\n", (long)DECODE_INT(t20));
    printf("%s\n", "=== Atom? et null? dans fonctions ===");
    describe(0);
    describe(nums);
    est_vide(0);
    est_vide(nums);
    printf("%s\n", "=== Construction et déconstruction ===");
    a = ENCODE_INT(1);
    /* [instruction non gérée explicitement] : IR_Assign */
    b = ENCODE_INT(2);
    /* [instruction non gérée explicitement] : IR_Assign */
    c = ENCODE_INT(3);
    /* [instruction non gérée explicitement] : IR_Assign */
    t21 = lisp_cons(c, 0);
    t22 = lisp_cons(b, t21);
    t23 = lisp_cons(a, t22);
    liste3 = t23;
    /* [instruction non gérée explicitement] : IR_Assign */
    printf("%s\n", "Liste (a b c), tete :");
    t24 = lisp_car(liste3);
    printf("%ld\n", (long)DECODE_INT(t24));
    printf("%s\n", "Queue de la liste, tete :");
    t25 = lisp_cdr(liste3);
    t26 = lisp_car(t25);
    printf("%ld\n", (long)DECODE_INT(t26));
    return 0;
}
