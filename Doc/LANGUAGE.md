# JulieSP — Documentation technique

Ce document décrit le fonctionnement interne du compilateur JulieSP, la grammaire du langage, la table des tokens et la bibliothèque standard.

Pour une référence rapide des tokens imprimable, voir [LEXEME.md](LEXEME.md).

---

## Pipeline de compilation

Un programme JulieSP passe par 5 étapes avant de devenir un binaire exécutable :

```
fichier.jlsp
     │
     ▼
┌─────────┐
│  Lexer  │  Découpe le code source en tokens
└─────────┘
     │
     ▼
┌─────────┐
│ Parser  │  Construit l'AST (arbre syntaxique abstrait)
└─────────┘
     │
     ▼
┌──────────────────┐
│ SemanticAnalyzer │  Vérifie les scopes et les symboles
└──────────────────┘
     │
     ▼
┌─────────────┐
│ IRGenerator │  Génère une représentation intermédiaire
└─────────────┘
     │
     ▼
┌─────────────┐
│ CGenerator  │  Produit le fichier C final
└─────────────┘
     │
     ▼
  output.c  ──► gcc ──► binaire exécutable
```

Chaque étape peut être inspectée avec les options `-dl`, `-da`, `-dr`, `-di`.

---

## Table des tokens

### Ignorés (skip)

| Token | Regex | Description |
|-------|-------|-------------|
| `DEL_SPACE` | `^[ \t\r\n]+` | Espaces, tabulations, retours à la ligne |
| `COM_LINE` | `^§§[^\n]*` | Commentaire sur une ligne |
| `COM_BLOCK` | `^§![\s\S]*?!§` | Commentaire multi-lignes |

### Littéraux

| Token | Symbole | Regex | Description |
|-------|---------|-------|-------------|
| `LIT_STRING` | `"..."` | `^"([^"\\]|\\.)*"` | Chaîne de caractères |
| `LIT_CHAR` | `'a'` | `^'([^'\\]|\\.)'` | Caractère unique |
| `LIT_INT` | `42` | `^-?[0-9]+` | Entier (positif ou négatif) |
| `LIT_FLOAT` | `3.14` | `^-?[0-9]+\.[0-9]+` | Flottant (séparateur : `.`) |
| `BOOL_TRUE` | `µ` | `^\xC2\xB5` ou `^\xCE\xBC` | Vrai (T) |
| `BOOL_FALSE` | `ù` | `^\xC3\xB9` | Faux / NIL |

### Délimiteurs

| Token | Symbole | Regex | Description |
|-------|---------|-------|-------------|
| `DEL_LBRACE` | `(` | `^\(` | Parenthèse ouvrante |
| `DEL_RBRACE` | `)` | `^\)` | Parenthèse fermante |

### Formes spéciales (noyau)

| Token | Symbole | Regex | Nb args | Description |
|-------|---------|-------|---------|-------------|
| `CORE_SETQ` | `:` | `^:` | 2 | Assignation de variable |
| `CORE_IF` | `?` | `^\?` | 2 ou 3 | Condition |
| `CORE_LAMBDA` | `£` | `^\xC2\xA3` | 2 | Fonction anonyme |
| `CORE_PROGN` | `;` | `^;` | n | Séquence d'expressions |
| `CORE_PRINT` | `€` | `^\xE2\x82\xAC` | 1 | Affichage |
| `CORE_SCAN` | `ç` | `^ç` | 1 | Saisie utilisateur (int uniquement) |
| `CORE_LOAD` | `$` | `^\$` | 1 | Chargement d'un fichier `.jlsp` |
| `CORE_QUOTE` | `²` | `^\xC2\xB2` | 1 | Citation — ne pas évaluer |

### Primitives de listes

| Token | Symbole | Regex | Nb args | Description |
|-------|---------|-------|---------|-------------|
| `MAIN_CAR` | `<<` | `^<<` | 1 | Premier élément d'une liste |
| `MAIN_CDR` | `>>` | `^>>` | 1 | Reste de la liste |
| `MAIN_CONS` | `&` | `^&` | 2 | Construire une liste |
| `MAIN_NULL` | `\|` | `^\|` | 1 | Vrai si liste vide |
| `MAIN_ATOM` | `@` | `^@` | 1 | Vrai si atome (pas une liste) |

### Opérateurs arithmétiques et comparaisons

| Token | Symbole | Regex | Nb args | Description |
|-------|---------|-------|---------|-------------|
| `CALC_PLUS` | `+` | `^\+` | n | Addition |
| `CALC_MOINS` | `-` | `^-` | 2 | Soustraction |
| `CALC_MULT` | `*` | `^\*` | n | Multiplication |
| `CALC_DIV` | `/` | `^\/` | 2 | Division entière |
| `CALC_INF` | `<` | `^<` | 2 | Inférieur à |
| `CALC_SUP` | `>` | `^>` | 2 | Supérieur à |
| `CALC_EQ` | `=` | `^=` | 2 | Égalité |
| `CALC_ADREQ` | `¤` | `^\xC2\xA4` | 2 | Test d'identité (adreq) |
| `CALC_NUMBERQ` | `°` | `^\xC2\xB0` | 1 | Vrai si l'argument est un nombre |

### Identifiants

| Token | Regex | Description |
|-------|-------|-------------|
| `IDENT` | `^[A-Za-z_][A-Za-z0-9_]*` | Nom de variable ou de fonction |

---

## Grammaire simplifiée

```
programme    ::= expression*
expression   ::= atome | s-expr
s-expr       ::= '(' forme ')'
forme        ::= setq | if | lambda | progn | print | scan | load | quote
               | arithmétique | comparaison | primitive-liste | appel

setq         ::= ':' IDENT expression
if           ::= '?' expression expression expression?
lambda       ::= '£' '(' IDENT* ')' expression
progn        ::= ';' expression+
print        ::= '€' expression
scan         ::= 'ç' IDENT
load         ::= '$' LIT_STRING
quote        ::= '²' expression

arithmétique ::= ('+' | '-' | '*' | '/') expression+
comparaison  ::= ('<' | '>' | '=') expression expression

car          ::= '<<' expression
cdr          ::= '>>' expression
cons         ::= '&' expression expression
null         ::= '|' expression
atom         ::= '@' expression

appel        ::= IDENT expression*
atome        ::= IDENT | LIT_INT | LIT_FLOAT | LIT_STRING | LIT_CHAR
               | BOOL_TRUE | BOOL_FALSE
```

---

## Notes sur le langage

### Variables

Toutes les variables déclarées au niveau global sont visibles depuis les fonctions. Les variables locales (dans un `£` ou un `;`) sont limitées à leur scope.

```lisp
(: x 100)
(: f (£ (n) (+ n x)))           §§ f peut accéder à x
```

### Fonctions

Une fonction est une lambda assignée à une variable. Le type de retour est toujours `lisp_obj`.

```lisp
(: carre (£ (n) (* n n)))
(€ (carre 5))                    §§ → 25
```

### Récursion

La récursion est supportée — le nom de la fonction est enregistré avant l'analyse du corps.

```lisp
(: factorielle (£ (n)
    (? (= n 0) 1 (* n (factorielle (- n 1))))))
```

### Listes

Les listes sont des listes chaînées construites avec `&`. Elles peuvent contenir des entiers et des chaînes. La liste vide s'écrit `²()`.

```lisp
(: lst (& 1 (& 2 (& 3 ²()))))
(€ (<< lst))                     §§ → 1
(€ (| ²()))                      §§ → 1
```

### Chargement de fichiers

`$` cherche le fichier relativement au répertoire du fichier source. Il est résolu à la compilation.

```lisp
($ "lib/julib.jlsp")
```

---

## Limitations connues

### Types et arithmétique
- **Flottants partiellement supportés** — un float peut être déclaré et affiché avec `€`, mais ne peut pas être retourné par une fonction (il sera traité comme un `lisp_obj` et corrompu par `ENCODE_INT`/`DECODE_INT`)
- **Arithmétique mixte int/float** — mélanger entiers et flottants dans une opération donne un résultat incorrect
- **Division entière uniquement** — `/` effectue une division entière même entre deux flottants

### Noms de variables
- **Pas de tirets** — les identifiants n'acceptent que `[A-Za-z_][A-Za-z0-9_]*`, donc `mon-var` est invalide, utiliser `mon_var`
- **Mots réservés C** — les noms suivants provoqueront une erreur gcc : `double`, `float`, `int`, `char`, `long`, `short`, `void`, `return`, `if`, `else`, `while`, `for`, `default`, `switch`

### Fonctions
- **Pas de fonctions d'ordre supérieur** — impossible de passer une fonction comme argument d'une autre fonction
- **Pas de closures** — les lambdas n'capturent pas leur environnement
- **Lambdas anonymes** — une lambda non assignée à une variable retourne NIL
- **Pas de types de retour déclarés** — toutes les fonctions retournent `lisp_obj`

### Entrées/Sorties
- **`ç` (scanf) Linux uniquement** — ne fonctionne pas sur Windows/macOS
- **`ç` entiers uniquement** — ne peut pas lire de strings ou de flottants
- **`€` (print)** — affiche un seul argument par appel; pour afficher plusieurs valeurs, utiliser plusieurs `€` ou un `progn`

### Listes
- **Pas de garbage collector** — les listes allouées avec `&` ne sont jamais libérées
- **Pas de listes hétérogènes complètes** — les listes peuvent contenir des entiers et des strings, mais pas des listes imbriquées de types mixtes de manière fiable

### Général
- **Pas de gestion des erreurs à l'exécution** — division par zéro, `car`/`cdr` sur liste vide provoquent un crash
- **Pas de types personnalisés** — pas de structs, records ou classes
- **Pas de modules** — `$` inclut tout le fichier dans le scope global
- **`?` sans else** — retourne NIL silencieusement si la condition est fausse et qu'il n'y a pas de branche else

---

## Bibliothèque standard — julib

Chargement :

```lisp
($ "lib/julib.jlsp")
```

### Arithmétique

| Fonction | Arguments | Description | Exemple |
|----------|-----------|-------------|---------|
| `abs` | `n` | Valeur absolue | `(abs -5)` → `5` |
| `max2` | `a b` | Maximum de deux valeurs | `(max2 3 7)` → `7` |
| `min2` | `a b` | Minimum de deux valeurs | `(min2 3 7)` → `3` |
| `carre` | `n` | Carré | `(carre 4)` → `16` |
| `cube` | `n` | Cube | `(cube 3)` → `27` |
| `pair` | `n` | Vrai si n est pair | `(pair 4)` → `1` |
| `impair` | `n` | Vrai si n est impair | `(impair 3)` → `1` |
| `clamp` | `val lo hi` | Limite val entre lo et hi | `(clamp 15 0 10)` → `10` |
| `puissance` | `base exp` | Puissance entière | `(puissance 2 8)` → `256` |
| `factorielle` | `n` | Factorielle récursive | `(factorielle 5)` → `120` |
| `fib` | `n` | Fibonacci récursif | `(fib 7)` → `13` |
| `somme_n` | `n` | Somme de 1 à n | `(somme_n 10)` → `55` |

### Logique

| Fonction | Arguments | Description | Exemple |
|----------|-----------|-------------|---------|
| `non` | `b` | Négation booléenne | `(non µ)` → `0` |
| `et` | `a b` | ET logique | `(et µ ù)` → `0` |
| `ou` | `a b` | OU logique | `(ou µ ù)` → `1` |
| `xou` | `a b` | OU exclusif | `(xou µ µ)` → `0` |

### Listes

| Fonction | Arguments | Description | Exemple |
|----------|-----------|-------------|---------|
| `longueur` | `l` | Nombre d'éléments | `(longueur lst)` → `3` |
| `deuxieme` | `l` | Deuxième élément | `(deuxieme lst)` → `2` |
| `troisieme` | `l` | Troisième élément | `(troisieme lst)` → `3` |
| `dernier` | `l` | Dernier élément | `(dernier lst)` → `30` |
| `contient` | `l x` | Vrai si x est dans l | `(contient lst 2)` → `1` |
| `somme_liste` | `l` | Somme des éléments | `(somme_liste lst)` → `60` |
| `max_liste` | `l` | Maximum de la liste | `(max_liste lst)` → `30` |
| `min_liste` | `l` | Minimum de la liste | `(min_liste lst)` → `10` |
| `nth` | `l n` | Nième élément (base 0) | `(nth lst 1)` → `20` |

### Exemple complet

```lisp
($ "lib/julib.jlsp")

(€ (factorielle 6))              §§ → 720
(€ (fib 10))                     §§ → 55
(€ (puissance 2 10))             §§ → 1024
(€ (clamp 150 0 100))            §§ → 100

(: lst (& 3 (& 1 (& 4 (& 1 (& 5 ²()))))))
(€ (longueur lst))               §§ → 5
(€ (max_liste lst))              §§ → 5
(€ (somme_liste lst))            §§ → 14
(€ (nth lst 2))                  §§ → 4
```

---

## Runtime C — `juliesp_runtime.h`

Le runtime fournit le type `lisp_obj` (`intptr_t`) et un système de tagging sur 2 bits :

| Tag (bits 1:0) | Type | Encodage |
|----------------|------|----------|
| `00` | `Node*` (liste) | pointeur aligné |
| `01` | entier | `ENCODE_INT(n)` = `(n << 2) \| 1` |
| `10` | `char*` (string) | `ENCODE_STR(s)` = `(s) \| 2` |

Macros principales :

```c
ENCODE_INT(n)     /* encode un entier              */
DECODE_INT(x)     /* décode un entier              */
ENCODE_STR(s)     /* encode une string             */
DECODE_STR(x)     /* décode une string             */
IS_INT(x)         /* vrai si entier                */
IS_STR(x)         /* vrai si string                */
IS_PTR(x)         /* vrai si Node* (liste)         */
IS_NIL(x)         /* vrai si NIL (0)               */
```

Fonctions de liste :

```c
lisp_obj lisp_cons(lisp_obj val, lisp_obj next)
lisp_obj lisp_car(lisp_obj lst)
lisp_obj lisp_cdr(lisp_obj lst)
int      lisp_null(lisp_obj lst)
int      lisp_atom(lisp_obj x)
int      lisp_numberp(lisp_obj x)
```

---

## Structure interne de l'AST

```
ASTNode (abstract)
├── Atom
│   ├── Symbol
│   │   ├── Primitive   (opérateurs : +, -, *, ?, :, £, ...)
│   │   └── Identifier  (noms de variables et fonctions)
│   └── Literal
│       ├── IntegerLit  (value: int)
│       ├── FloatLit    (value: float)
│       ├── CharLit     (value: char)
│       ├── StringLit   (value: string)
│       └── BoolLit     (value: bool)
└── SExpr               (children: vector<ASTNode*>)
```

Tous les nœuds héritent de `ASTNode` et portent `line`, `col`, et `isQuoted`.
