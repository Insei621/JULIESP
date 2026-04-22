/*
 * lisp_runtime.h — Runtime C pour le langage JulieSP
 *
 * Ce fichier définit les primitives Lisp compilées en fonctions C.
 * Il est inclus automatiquement dans tout fichier généré par CGenerator.
 *
 * On utilise une représentation "tagged union" minimaliste pour les valeurs
 * Lisp dynamiques (car/cdr/cons ont besoin de pouvoir travailler sur
 * n'importe quel type). Pour les programmes simples (sans listes), ces
 * fonctions ne sont pas appelées.
 *
 * STRUCTURE D'UNE VALEUR LISP :
 *
 *   typedef struct LispVal LispVal;
 *
 *   Un LispVal est soit :
 *     - Un entier    (tag = LISP_INT)
 *     - Un flottant  (tag = LISP_FLOAT)
 *     - Un caractère (tag = LISP_CHAR)
 *     - Une chaîne   (tag = LISP_STRING)
 *     - Un booléen   (tag = LISP_BOOL)
 *     - Une paire    (tag = LISP_PAIR) : head + tail
 *     - NIL          (tag = LISP_NIL)  : la liste vide
 */

#ifndef LISP_RUNTIME_H
#define LISP_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================================
 * 1. Le type LispVal — la valeur universelle
 * ============================================================================ */

typedef enum {
    LISP_INT,
    LISP_FLOAT,
    LISP_CHAR,
    LISP_STRING,
    LISP_BOOL,
    LISP_PAIR,
    LISP_NIL
} LispTag;

typedef struct LispVal {
    LispTag tag;
    union {
        int         i;       /* LISP_INT, LISP_BOOL */
        float       f;       /* LISP_FLOAT */
        char        c;       /* LISP_CHAR */
        char*       s;       /* LISP_STRING */
        struct {             /* LISP_PAIR */
            struct LispVal* head;
            struct LispVal* tail;
        } pair;
    };
} LispVal;

/* ============================================================================
 * 2. Constructeurs — créent un LispVal alloué sur le tas
 * ============================================================================
 *
 * NOTE : Pour un vrai compilateur, on voudrait un GC. Ici on alloue avec
 * malloc et on ne libère pas (acceptable pour des programmes courts).
 */

static inline LispVal* lisp_make_int(int v) {
    LispVal* val = (LispVal*)malloc(sizeof(LispVal));
    val->tag = LISP_INT; val->i = v; return val;
}

static inline LispVal* lisp_make_float(float v) {
    LispVal* val = (LispVal*)malloc(sizeof(LispVal));
    val->tag = LISP_FLOAT; val->f = v; return val;
}

static inline LispVal* lisp_make_char(char v) {
    LispVal* val = (LispVal*)malloc(sizeof(LispVal));
    val->tag = LISP_CHAR; val->c = v; return val;
}

static inline LispVal* lisp_make_string(const char* v) {
    LispVal* val = (LispVal*)malloc(sizeof(LispVal));
    val->tag = LISP_STRING;
    val->s = (char*)malloc(strlen(v) + 1);
    strcpy(val->s, v);
    return val;
}

static inline LispVal* lisp_make_bool(int v) {
    LispVal* val = (LispVal*)malloc(sizeof(LispVal));
    val->tag = LISP_BOOL; val->i = (v != 0); return val;
}

/* La valeur NIL est un singleton global */
static LispVal LISP_NIL_VAL = { LISP_NIL, {0} };
static inline LispVal* lisp_nil(void) { return &LISP_NIL_VAL; }

/* ============================================================================
 * 3. Primitives Lisp
 * ============================================================================ */

/*
 * cons : crée une paire (head . tail)
 * Exemple Lisp : (cons 1 '(2 3)) → (1 2 3)
 */
static inline LispVal* lisp_cons(LispVal* head, LispVal* tail) {
    LispVal* val = (LispVal*)malloc(sizeof(LispVal));
    val->tag = LISP_PAIR;
    val->pair.head = head;
    val->pair.tail = tail;
    return val;
}

/*
 * car : retourne le premier élément d'une paire
 * Exemple Lisp : (car '(1 2 3)) → 1
 */
static inline LispVal* lisp_car(LispVal* val) {
    if (val->tag != LISP_PAIR) {
        fprintf(stderr, "ERREUR RUNTIME : car appliqué à un non-paire\n");
        exit(1);
    }
    return val->pair.head;
}

/*
 * cdr : retourne le reste d'une paire
 * Exemple Lisp : (cdr '(1 2 3)) → (2 3)
 */
static inline LispVal* lisp_cdr(LispVal* val) {
    if (val->tag != LISP_PAIR) {
        fprintf(stderr, "ERREUR RUNTIME : cdr appliqué à un non-paire\n");
        exit(1);
    }
    return val->pair.tail;
}

/*
 * null? : vrai si la valeur est NIL
 * Exemple Lisp : (null? '()) → #t
 */
static inline int lisp_null_(LispVal* val) {
    return val->tag == LISP_NIL;
}

/*
 * atom? : vrai si la valeur n'est PAS une paire
 * Exemple Lisp : (atom? 42) → #t, (atom? '(1 2)) → #f
 */
static inline int lisp_atom_(LispVal* val) {
    return val->tag != LISP_PAIR;
}

/*
 * number? : vrai si la valeur est un int ou float
 */
static inline int lisp_number_(LispVal* val) {
    return val->tag == LISP_INT || val->tag == LISP_FLOAT;
}

/* ============================================================================
 * 4. Utilitaire d'affichage (utilisé par IR_Print sur un LispVal*)
 * ============================================================================ */

static inline void lisp_print(LispVal* val) {
    if (!val) { printf("nil"); return; }
    switch (val->tag) {
        case LISP_INT:    printf("%d",  val->i); break;
        case LISP_FLOAT:  printf("%f",  val->f); break;
        case LISP_CHAR:   printf("%c",  val->c); break;
        case LISP_STRING: printf("%s",  val->s); break;
        case LISP_BOOL:   printf("%s",  val->i ? "#t" : "#f"); break;
        case LISP_NIL:    printf("nil"); break;
        case LISP_PAIR:
            printf("(");
            lisp_print(val->pair.head);
            /* Affiche le reste de la liste proprement */
            LispVal* rest = val->pair.tail;
            while (rest->tag == LISP_PAIR) {
                printf(" ");
                lisp_print(rest->pair.head);
                rest = rest->pair.tail;
            }
            if (rest->tag != LISP_NIL) {
                printf(" . ");
                lisp_print(rest);
            }
            printf(")");
            break;
    }
}

#endif /* LISP_RUNTIME_H */
