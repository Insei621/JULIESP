/* Code généré automatiquement — ne pas éditer */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./juliesp_runtime.h"

/* --- Prototypes --- */
lisp_obj au_carre(lisp_obj n);
lisp_obj calculer_score(lisp_obj score_base);
lisp_obj tester_scopes(lisp_obj a);
lisp_obj factorielle(lisp_obj n);
lisp_obj analyser_liste(lisp_obj l);
lisp_obj test_quote(lisp_obj l);

/* --- Variables globales --- */
lisp_obj x;
lisp_obj t0;
lisp_obj a;
lisp_obj t5;
lisp_obj t6;
lisp_obj b;
char* message;
lisp_obj t7;
lisp_obj t11;
lisp_obj t12;
lisp_obj t13;
lisp_obj ma_liste;
lisp_obj t15;
lisp_obj reste;
lisp_obj t16;
lisp_obj t19;
lisp_obj resultat;
lisp_obj t27;
lisp_obj t28;
lisp_obj t29;
lisp_obj t30;
lisp_obj complexe;
lisp_obj t31;
lisp_obj t32;
lisp_obj multiplicateur_universel;
lisp_obj t53;
lisp_obj t55;
lisp_obj t62;
lisp_obj t63;
lisp_obj t64;
lisp_obj t65;
lisp_obj t72;
lisp_obj t73;
lisp_obj t74;
lisp_obj t75;

lisp_obj au_carre(lisp_obj n) {
    lisp_obj t18;

    t18 = n * n;
    return t18;
}

lisp_obj calculer_score(lisp_obj score_base) {
    lisp_obj bonus;
    lisp_obj t51;
    lisp_obj total;
    lisp_obj t52;

    bonus = ENCODE_INT(50);
    t51 = score_base + bonus;
    total = t51;
    t52 = total * multiplicateur_universel;
    return t52;
}

lisp_obj tester_scopes(lisp_obj a) {
    lisp_obj t56;
    lisp_obj b;

    t56 = a + x;
    b = t56;
    printf("%ld\n", (long)DECODE_INT(b));
    return 0; // NIL par défaut
}

lisp_obj factorielle(lisp_obj n) {
    lisp_obj t57;
    lisp_obj t58;
    lisp_obj t59;
    lisp_obj t60;
    lisp_obj t61;

    t57 = n == 0;
    if (t57) {
        t58 = 1;
    } else {
        t59 = n - 1;
        t60 = factorielle(t59);
        t61 = n * t60;
        t58 = t61;
    }
    return t58;
}

lisp_obj analyser_liste(lisp_obj l) {
    lisp_obj t66;
    lisp_obj t68;
    lisp_obj t69;
    lisp_obj t70;
    lisp_obj t71;
    lisp_obj reste;

    t66 = lisp_null(l);
    if (t66) {
        printf("%s\n", "Liste vide");
    } else {
        t68 = lisp_car(l);
        printf("%ld\n", (long)DECODE_INT(t68));
        t69 = lisp_car(l);
        t70 = lisp_atom(t69);
        printf("%ld\n", (long)DECODE_INT(t70));
        t71 = lisp_cdr(l);
        reste = t71;
        printf("%ld\n", (long)DECODE_INT(reste));
    }
    return 0; // NIL par défaut
}

lisp_obj test_quote(lisp_obj l) {
    lisp_obj y;

    y = ENCODE_INT("x");
    printf("%s\n", "La variable x vaut :");
    printf("%ld\n", (long)DECODE_INT(x));
    printf("%s\n", "Le symbole cité y est :");
    printf("%ld\n", (long)DECODE_INT(y));
    return 0;
}

int main(void) {
    x = ENCODE_INT(42);
    t0 = x < 50;
    if (t0) {
        printf("%s\n", "Petit");
    } else {
        printf("%s\n", "Grand");
    }
    a = ENCODE_INT(10);
    t5 = 5 * 2.500000f;
    t6 = a + t5;
    b = t6;
    printf("%ld\n", (long)DECODE_INT(b));
    message = ENCODE_STR("Resultat:");
    printf("%s\n", message);
    t7 = a > 18;
    if (t7) {
        printf("%s\n", "Acces autorise");
    } else {
        printf("%s\n", "Acces refuse");
    }
    t11 = lisp_cons(ENCODE_INT(3), 0);
    t12 = lisp_cons(ENCODE_INT(2), t11);
    t13 = lisp_cons(ENCODE_INT(1), t12);
    ma_liste = t13;
    t15 = lisp_cdr(ma_liste);
    reste = t15;
    t16 = lisp_null(reste);
    if (t16) {
        printf("%s\n", "Fin de liste");
    } else {
        printf("%s\n", "Il en reste encore");
    }
    printf("%s\n", "Calcul en cours...");
    t19 = au_carre(ENCODE_INT(8));
    resultat = t19;
    printf("%s\n", "Le carre de 8 est :");
    printf("%s\n", "Bonjour ");
    t27 = 1 + 2;
    if (1) {
        t28 = 42;
    } else {
        t28 = 0;
    }
    t29 = lisp_cons(t28, 0);
    t30 = lisp_cons(t27, t29);
    complexe = t30;
    t31 = lisp_car(complexe);
    t32 = lisp_atom(t31);
    printf("%ld\n", (long)DECODE_INT(t32));
    x = 0.000100f;
    printf("%s\n", "Valeur finale de y :");
    if (1) {
        printf("%s\n", "Vrai");
    }
    printf("%s\n", "Suite du code");
    multiplicateur_universel = ENCODE_INT(10);
    t53 = multiplicateur_universel > 0;
    if (t53) {
        printf("%s\n", "Calcul en cours...");
        t55 = calculer_score(ENCODE_INT(100));
        resultat = t55;
        printf("%s\n", "Le résultat final est prêt.");
    } else {
        printf("%s\n", "Erreur : Constante universelle invalide.");
    }
    x = ENCODE_INT(100);
    tester_scopes(ENCODE_INT(50));
    t62 = factorielle(ENCODE_INT(5));
    resultat = t62;
    printf("%s\n", "Factorielle de 5 est :");
    printf("%ld\n", (long)DECODE_INT(resultat));
    t63 = lisp_cons(ENCODE_INT(30), 0);
    t64 = lisp_cons(ENCODE_INT(20), t63);
    t65 = lisp_cons(ENCODE_INT(10), t64);
    ma_liste = t65;
    analyser_liste(ma_liste);
    x = ENCODE_INT(10);
    t72 = 5 + 5;
    t73 = lisp_cons(0, 0);
    t74 = lisp_cons(t72, t73);
    t75 = lisp_cons(ENCODE_STR("test"), t74);
    ma_liste = t75;
    return 0;
}
