# JuliesSP

Compilateur pour le langage **JuliesSP**, un dialecte Lisp qui compile vers C.

---

## Installation

### Prérequis

- `cmake` (>= 3.16)
- `g++` avec support C++20
- `gcc` pour compiler le code généré
- `graphviz` (optionnel, pour `-di`)

Sur Ubuntu/Debian :
```bash
sudo apt install cmake g++ gcc graphviz
```

### Installer JuliesSP

```bash
git clone https://github.com/Insei621/JULIESP
cd JULIESP
chmod +x install.sh
./install.sh
```

Après l'installation, `juliesp` est accessible depuis n'importe où dans le terminal.

---

## Utilisation

```bash
juliesp <fichier.jlsp> [options]
```

### Options de sortie

| Court | Long | Description |
|-------|------|-------------|
| `-o <fichier.c>` | `--output <fichier.c>` | Fichier de sortie C (défaut: `output/output.c`) |
| `-c` | `--compile` | Compile le C généré avec gcc en un binaire exécutable |

### Options de debug

| Court | Long | Description |
|-------|------|-------------|
| `-dl` | `--dump-lex` | Affiche les tokens (analyse lexicale) |
| `-da` | `--dump-ast` | Affiche l'AST dans le terminal |
| `-di` | `--dump-imgast` | Génère `AST_Graphe/ast.png` via Graphviz |
| `-dr` | `--dump-ir` | Affiche la représentation intermédiaire |

### Options de pipeline

| Option | Description |
|--------|-------------|
| `--lex-only` | S'arrête après le lexage |
| `--parse-only` | S'arrête après le parsing |
| `--sem-only` | S'arrête après l'analyse sémantique |
| `--ir-only` | S'arrête après la génération IR |
| `-h`, `--help` | Affiche l'aide |

### Exemples

```bash
# Compiler un programme JuliesSP vers C
juliesp programme.jlsp

# Compiler vers un fichier C spécifique
juliesp programme.jlsp -o mon_prog.c

# Compiler vers C puis produire un binaire exécutable
juliesp programme.jlsp -c

# Compiler vers un fichier spécifique et produire un binaire
juliesp programme.jlsp -o mon_prog.c -c

# Afficher les tokens et l'IR
juliesp programme.jlsp -dl -dr

# Afficher l'AST dans le terminal
juliesp programme.jlsp -da

# Générer une image de l'AST
juliesp programme.jlsp -di

# S'arrêter après l'analyse sémantique
juliesp programme.jlsp --sem-only
```

---

## Syntaxe JuliesSP

### Formes spéciales

| JuliesSP | Lisp standard | Description |
|----------|----------|-------------|
| `: x 42` | `setq x 42` | Assignation de variable |
| `? cond then else` | `if cond then else` | Condition |
| `£ (params) corps` | `lambda (params) corps` | Fonction anonyme |
| `; expr1 expr2` | `progn expr1 expr2` | Séquence d'expressions |
| `€ expr` | `print expr` | Affichage |
| `ç var` | `scanf var` | Saisie utilisateur (entiers) |
| `$ "fichier"` | `load "fichier"` | Chargement de fichier |
| `² expr` | `' expr` | Citation (ne pas évaluer) |

### Primitives de listes

| JuliesSP | Lisp standard | Description |
|----------|---------------|-------------|
| `<< lst` | `car lst` | Premier élément |
| `>> lst` | `cdr lst` | Reste de la liste |
| `& val lst` | `cons val lst` | Construire une liste |
| `\| lst` | `null? lst` | Vrai si liste vide |
| `@ val` | `atom? val` | Vrai si atome (pas une liste) |

### Opérateurs arithmétiques

| Opérateur | Description |
|-----------|-------------|
| `+`, `-`, `*`, `/` | Opérations de base |
| `<`, `>`, `=` | Comparaisons |
| `¤ a b` | Égalité (adreq) |
| `° x` | Test nombre (numberq) |

### Littéraux

| JuliesSP | Description |
|----------|-------------|
| `42` | Entier |
| `3.14` | Flottant |
| `"hello"` | Chaîne |
| `µ` | Vrai (T) |
| `ù` | Faux / NIL |
| `²()` | Liste vide |

### Commentaires

```lisp
§§ Commentaire sur une ligne

§!
  Commentaire
  multi-lignes
!§
```

### Exemple — Factorielle

```lisp
(: factorielle (£ (n)
    (? (= n 0)
        1
        (* n (factorielle (- n 1))))))

(: resultat (factorielle 5))
(€ "Factorielle de 5 :")
(€ resultat)
```

### Exemple — Listes

```lisp
(: lst (& 10 (& 20 (& 30 ²()))))

(€ (<< lst))           §§ → 10
(€ (<< (>> lst)))      §§ → 20
(€ (| ²()))            §§ → 1 (liste vide)
(€ (@ 42))             §§ → 1 (c'est un atome)
```

---

## Tests

Le dossier `tests/` contient une suite de tests unitaires couvrant toutes les fonctionnalités du langage.

### Lancer tous les tests

```bash
cd tests
chmod +x run_tests.sh
./run_tests.sh
```

Si `juliesp` n'est pas encore installé globalement :

```bash
./run_tests.sh /chemin/vers/cmake-build-debug/juliesp
```

### Résultat attendu

```
  JuliesSP — Suite de tests unitaires
  =====================================

  [PASS] unit_01_variables.jlsp
  [PASS] unit_02_arithmetique.jlsp
  [PASS] unit_03_conditions.jlsp
  [PASS] unit_04_lambda.jlsp
  [PASS] unit_05_progn.jlsp
  [PASS] unit_06_listes.jlsp
  [PASS] unit_07_recursion.jlsp
  [PASS] unit_08_scopes.jlsp
  [PASS] unit_09_listes_fonctions.jlsp
  [PASS] unit_10_strings.jlsp
  [PASS] unit_11_primitives.jlsp

  =====================================
  Résultats : 11 passés, 0 échoués
```

### Liste des tests

| Fichier | Ce qu'il teste | Cas |
|---------|----------------|-----|
| `unit_01_variables` | Variables, types, réassignation | 7 |
| `unit_02_arithmetique` | +, -, *, /, comparaisons | 10 |
| `unit_03_conditions` | if/else, imbrications | 9 |
| `unit_04_lambda` | Fonctions, appels, composition | 7 |
| `unit_05_progn` | Séquences d'instructions | 4 |
| `unit_06_listes` | cons, car, cdr, null?, atom? | 9 |
| `unit_07_recursion` | Factorielle, Fibonacci, somme | 4 |
| `unit_08_scopes` | Variables globales, shadowing | 4 |
| `unit_09_listes_fonctions` | Primitives listes dans fonctions | 7 |
| `unit_10_strings` | Chaînes de caractères | 5 |
| `unit_11_primitives` | °, ¤, <, >, = | 7 |

---

## Runtime

Le fichier `juliesp_runtime.h` est installé dans `/usr/local/include`. Il est automatiquement inclus dans tout fichier C généré et fournit le support pour les listes Lisp via un système de valeurs taguées (`lisp_obj`).

Les listes peuvent contenir des **entiers** et des **chaînes de caractères**.

---

