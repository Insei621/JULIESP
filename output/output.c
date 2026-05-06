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

lisp_obj au_carre(lisp_obj n) {
    lisp_obj t18;

    t18 = n * n;
    return t18;
}

lisp_obj calculer_score(lisp_obj score_base) {
    lisp_obj bonus;
    lisp_obj t59;
    lisp_obj total;
    lisp_obj t60;

    bonus = ENCODE_INT(50);
    t59 = score_base + bonus;
    total = t59;
    t60 = total * multiplicateur_universel;
    return t60;
}

lisp_obj tester_scopes(lisp_obj a) {
    lisp_obj t64;
    lisp_obj b;

    t64 = a + x;
    b = t64;
    printf("%ld\n", (long)DECODE_INT(b));
    return 0; // NIL par défaut
}

lisp_obj factorielle(lisp_obj n) {
    lisp_obj t65;
    lisp_obj t66;
    lisp_obj t67;
    lisp_obj t68;
    lisp_obj t69;

    t65 = n == 0;
    if (t65) {
        t66 = 1;
    } else {
        t67 = n - 1;
        t68 = factorielle(t67);
        t69 = n * t68;
        t66 = t69;
    }
    return t66;
}

lisp_obj analyser_liste(lisp_obj l) {
    lisp_obj t74;
    lisp_obj t76;
    lisp_obj t77;
    lisp_obj t78;
    lisp_obj t79;
    lisp_obj reste;

    t74 = lisp_null(l);
    if (t74) {
        printf("%s\n", "Liste vide");
    } else {
        t76 = lisp_car(l);
        printf("%ld\n", (long)DECODE_INT(t76));
        t77 = lisp_car(l);
        t78 = lisp_atom(t77);
        printf("%ld\n", (long)DECODE_INT(t78));
        t79 = lisp_cdr(l);
        reste = t79;
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
    return __fn1;
}

int main(void) {
    lisp_obj x;
    lisp_obj t0;
    lisp_obj t2;
    lisp_obj t3;
    lisp_obj a;
    lisp_obj t5;
    lisp_obj t6;
    lisp_obj b;
    char* message;
    lisp_obj t7;
    lisp_obj t9;
    lisp_obj t10;
    lisp_obj t11;
    lisp_obj t12;
    lisp_obj t13;
    lisp_obj ma_liste;
    lisp_obj t14;
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
    lisp_obj t33;
    lisp_obj t34;
    lisp_obj t35;
    lisp_obj t36;
    lisp_obj t37;
    lisp_obj t38;
    lisp_obj t39;
    lisp_obj t40;
    lisp_obj t41;
    lisp_obj t42;
    lisp_obj t43;
    lisp_obj t44;
    lisp_obj t45;
    lisp_obj y;
    lisp_obj t46;
    lisp_obj t47;
    lisp_obj t48;
    lisp_obj t49;
    lisp_obj t50;
    lisp_obj t55;
    lisp_obj t51;
    lisp_obj t56;
    lisp_obj t57;
    lisp_obj multiplicateur_universel;
    lisp_obj t61;
    lisp_obj t63;
    lisp_obj t70;
    lisp_obj t71;
    lisp_obj t72;
    lisp_obj t73;
    lisp_obj t80;
    lisp_obj t81;
    lisp_obj t82;
    lisp_obj t83;

    x = ENCODE_INT(42);
    t0 = x < 50;
    if (t0) {
        printf("%s\n", "Petit");
    } else {
        printf("%s\n", "Grand");
    }
    t2 = 1 + 2;
    t3 = 3 * 4;
    t4 = t2 + t3;
    a = ENCODE_INT(10);
    t5 = 5 * 2.500000f;
    t6 = a + t5;
    b = t6;
    printf("%ld\n", (long)DECODE_INT(b));
    message = "Resultat:";
    printf("%ld\n", (long)DECODE_INT(message));
    t7 = a > 18;
    if (t7) {
        printf("%s\n", "Acces autorise");
    } else {
        printf("%s\n", "Acces refuse");
    }
    t9 = lisp_numberp(a);
    t10 = a == 10;
    t11 = lisp_cons(ENCODE_INT(3), ENCODE_INT(0));
    t12 = lisp_cons(ENCODE_INT(2), t11);
    t13 = lisp_cons(ENCODE_INT(1), t12);
    ma_liste = t13;
    t14 = lisp_car(ma_liste);
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
    t33 = 1 + 2;
    t34 = t33 + 3;
    t35 = t34 + 4;
    t36 = t35 + 5;
    t37 = t36 + 6;
    t38 = t37 + 7;
    t39 = t38 + 8;
    t40 = t39 + 9;
    t41 = 2 * 5;
    t42 = t41 * 10;
    t43 = 3 * 3;
    t44 = t42 + t43;
    t45 = t44 + 1;
    y = ENCODE_INT(10);
    t46 = y + 1;
    y = t46;
    t47 = y * 2;
    y = t47;
    printf("%s\n", "Valeur finale de y :");
    t48 = 2 * 3;
    t49 = 1 + t48;
    t50 = t49 > 5;
    if (t50) {
        t55 = lisp_cons("resultat", __fn0);
        t51 = t55;
    } else {
        printf("%s\n", "Erreur");
        t51 = 0;
    }
    t56 = lisp_cons(0, 0);
    t57 = lisp_cons(0, t56);
    if (1) {
        printf("%s\n", "Vrai");
    }
    printf("%s\n", "Suite du code");
    multiplicateur_universel = ENCODE_INT(10);
    t61 = multiplicateur_universel > 0;
    if (t61) {
        printf("%s\n", "Calcul en cours...");
        t63 = calculer_score(ENCODE_INT(100));
        resultat = t63;
        printf("%s\n", "Le résultat final est prêt.");
    } else {
        printf("%s\n", "Erreur : Constante universelle invalide.");
    }
    x = ENCODE_INT(100);
    tester_scopes(ENCODE_INT(50));
    t70 = factorielle(ENCODE_INT(5));
    resultat = t70;
    printf("%s\n", "Factorielle de 5 est :");
    printf("%ld\n", (long)DECODE_INT(resultat));
    t71 = lisp_cons(ENCODE_INT(30), ENCODE_INT(0));
    t72 = lisp_cons(ENCODE_INT(20), t71);
    t73 = lisp_cons(ENCODE_INT(10), t72);
    ma_liste = t73;
    analyser_liste(ma_liste);
    x = ENCODE_INT(10);
    t80 = 5 + 5;
    t81 = lisp_cons(0, ENCODE_INT(0));
    t82 = lisp_cons(t80, t81);
    t83 = lisp_cons("test", t82);
    ma_liste = t83;
    t84 = test_quote(ma_liste);
    return 0;
}
