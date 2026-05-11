# JULIESP

Compilateur pour le langage **Juliesp**, un dialecte Lisp qui compile vers C.

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

### Installer JULIESP

Collez les commandes suivantes dans un terminal bash et le compilateur sera installé sur votre machine.
```bash
git clone https://github.com/Insei621/juliesp
cd juliesp
sudo make install
```
**Attention** : si vous exécutez dans Docker, le sudo n'est pas disponible, faites d'abord:
```bash
cd build/juliesp
make install
```

Après l'installation, `juliesp` est accessible depuis n'importe où dans le terminal.

### Commandes disponibles

| Commande | Description |
|----------|-------------|
| `make` | Compile le projet |
| `sudo make install` | Compile et installe |
| `make clean` | Supprime les fichiers de compilation |
| `sudo make uninstall` | Désinstalle juliesp |

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
| `-b <nom>` | `--binary <nom>` | Nom du binaire exécutable (défaut: nom du fichier C sans `.c`) |
| `-r` | `--run` | Exécute le binaire après compilation (implique `-c`) |

### Options de debug

| Court | Long | Description |
|-------|------|-------------|
| `-d` | `--debug` | Active tous les dumps (`-dl -da -di -dr`) |
| `-dl` | `--dump-lex` | Affiche les tokens (analyse lexicale) |
| `-da` | `--dump-ast` | Affiche l'AST dans le terminal |
| `-di` | `--dump-imgast` | Génère `ast.png` dans le dossier de sortie via Graphviz |
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
# Compiler un programme JULIESP vers C
juliesp programme.jlsp

# Compiler vers un fichier C spécifique
juliesp programme.jlsp -o mon_prog.c

# Compiler vers C puis produire un binaire exécutable
juliesp programme.jlsp -c

# Compiler avec un nom de binaire spécifique
juliesp programme.jlsp -c -b mon_programme

# Compiler et exécuter directement
juliesp programme.jlsp -r

# Compiler, nommer le binaire et exécuter
juliesp programme.jlsp -o mon_prog.c -b mon_prog -r

# Activer tous les dumps d'un coup
juliesp programme.jlsp -d

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

## Manuel

Après installation, la page de manuel complète est accessible dans le terminal :

```bash
man juliesp
```

Elle contient la liste complète des options, la syntaxe du langage, les primitives de listes et la bibliothèque standard.

---

## Documentation

| Fichier | Contenu |
|---------|---------|
| [LANGUAGE.md](LANGUAGE.md) | Documentation technique complète — pipeline, tokens, grammaire, limitations, julib, runtime |
| [LEXEME.md](LEXEME.md) | Référence rapide des tokens — imprimable |
| [LEXEME_JULIB.md](LEXEME_JULIB.md) | Référence rapide des fonctions julib — imprimable |

> Pour aller plus loin que ce README, consultez `LANGUAGE.md`.
 
---

## Syntaxe JULIESP

### Formes spéciales

| JULIESP | Lisp standard | Description |
|---------|---------------|-------------|
| `: x 42` | `setq x 42` | Assignation de variable |
| `? cond then else` | `if cond then else` | Condition |
| `£ (params) corps` | `lambda (params) corps` | Fonction anonyme |
| `; expr1 expr2` | `progn expr1 expr2` | Séquence d'expressions |
| `€ expr` | `print expr` | Affichage |
| `ç var` | `scanf var` | Saisie utilisateur (entiers) |
| `$ "fichier"` | `load "fichier"` | Chargement de fichier |
| `² expr` | `quote expr` | Citation (ne pas évaluer) |

### Primitives de listes

| JULIESP | Lisp standard | Description |
|---------|---------------|-------------|
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

| JULIESP | Description |
|---------|-------------|
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

Le dossier `tests_bench/` contient une suite de tests unitaires couvrant toutes les fonctionnalités du langage.

### Lancer tous les tests

```bash
cd tests_bench
./run_tests.sh
```

Si `juliesp` n'est pas encore installé globalement :

```bash
./run_tests.sh /chemin/vers/cmake-build-debug/juliesp
```

### Résultat attendu

```
  JULIESP — Suite de tests unitaires
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
| `tests_bench/unit_01_variables` | Variables, types, réassignation | 7 |
| `tests_bench/unit_02_arithmetique` | +, -, *, /, comparaisons | 10 |
| `tests_bench/unit_03_conditions` | if/else, imbrications | 9 |
| `tests_bench/unit_04_lambda` | Fonctions, appels, composition | 7 |
| `tests_bench/unit_05_progn` | Séquences d'instructions | 4 |
| `tests_bench/unit_06_listes` | cons, car, cdr, null?, atom? | 9 |
| `tests_bench/unit_07_recursion` | Factorielle, Fibonacci, somme | 4 |
| `tests_bench/unit_08_scopes` | Variables globales, shadowing | 4 |
| `tests_bench/unit_09_listes_fonctions` | Primitives listes dans fonctions | 7 |
| `tests_bench/unit_10_strings` | Chaînes de caractères | 5 |
| `tests_bench/unit_11_primitives` | °, ¤, <, >, = | 7 |

---

### Tests d'erreur

Le dossier `tests_bench/` contient également une suite de tests vérifiant que le compilateur produit les bons messages d'erreur.

```bash
cd tests_bench
chmod +x run_error_tests.sh
./run_error_tests.sh
```

| Fichier | Erreur testée |
|---------|---------------|
| `err_01_lexer_caractere_inconnu` | Caractère non reconnu par le lexer |
| `err_02_syntaxe_parenthese_manquante` | Parenthèse fermante manquante |
| `err_03_syntaxe_setq_sans_valeur` | Assignation sans valeur |
| `err_04_semantique_variable_non_definie` | Variable non définie |
| `err_05_semantique_car_sur_litteral` | `<<` appliqué à un entier |
| `err_06_semantique_cdr_sur_litteral` | `>>` appliqué à un entier |
| `err_07_semantique_null_sur_litteral` | `\|` appliqué à un entier |
| `err_08_semantique_cons_mauvais_arg` | `&` avec entier comme 2ème argument |
| `err_09_semantique_variable_hors_scope` | Variable locale utilisée hors de son scope |
| `err_10_load_fichier_introuvable` | Fichier `$` introuvable |

---

## Exemples

Le dossier `examples/` contient des programmes JULIESP progressifs pour apprendre le langage.

```bash
juliesp examples/01_hello_world.jlsp -r
```

| Fichier | Concept illustré |
|---------|-----------------|
| `01_hello_world` | Premier programme |
| `02_variables` | Types et assignations |
| `03_arithmetique` | Opérations et comparaisons |
| `04_conditions` | if/else, imbrications |
| `05_fonctions` | Lambda, appels, composition |
| `06_recursion` | Factorielle, Fibonacci, somme |
| `07_listes` | cons, car, cdr, null?, atom? |
| `08_progn_scopes` | Séquences et portée des variables |
| `09_calculatrice` | Programme complet avec saisie utilisateur |
| `10_julib` | Bibliothèque standard complète |

---

## Bibliothèque standard — julib

JULIESP inclut une bibliothèque standard `julib.jlsp` dans le dossier `lib/`. Pour la référence complète des fonctions, voir [LEXEME_JULIB.md](LEXEME_JULIB.md).

### Utilisation

```lisp
($ "lib/julib.jlsp")

(€ (factorielle 5))
(€ (longueur (& 1 (& 2 (& 3 ²())))))
(€ (max2 10 42))
```

### Fonctions disponibles

#### Arithmétique

| Fonction | Description | Exemple |
|----------|-------------|---------|
| `(abs n)` | Valeur absolue | `(abs -5)` → `5` |
| `(max2 a b)` | Maximum de deux valeurs | `(max2 3 7)` → `7` |
| `(min2 a b)` | Minimum de deux valeurs | `(min2 3 7)` → `3` |
| `(carre n)` | Carré | `(carre 4)` → `16` |
| `(cube n)` | Cube | `(cube 3)` → `27` |
| `(pair n)` | Vrai si n est pair | `(pair 4)` → `1` |
| `(impair n)` | Vrai si n est impair | `(impair 3)` → `1` |
| `(clamp val lo hi)` | Limite val entre lo et hi | `(clamp 15 0 10)` → `10` |
| `(puissance base exp)` | Puissance entière | `(puissance 2 8)` → `256` |
| `(factorielle n)` | Factorielle | `(factorielle 5)` → `120` |
| `(fib n)` | Fibonacci | `(fib 7)` → `13` |
| `(somme_n n)` | Somme de 1 à n | `(somme_n 10)` → `55` |

#### Logique

| Fonction | Description | Exemple |
|----------|-------------|---------|
| `(non b)` | Négation booléenne | `(non µ)` → `0` |
| `(et a b)` | ET logique | `(et µ ù)` → `0` |
| `(ou a b)` | OU logique | `(ou µ ù)` → `1` |
| `(xou a b)` | OU exclusif | `(xou µ µ)` → `0` |

#### Listes

| Fonction | Description | Exemple |
|----------|-------------|---------|
| `(longueur l)` | Longueur d'une liste | `(longueur lst)` → `3` |
| `(deuxieme l)` | Deuxième élément | `(deuxieme lst)` → `2` |
| `(troisieme l)` | Troisième élément | `(troisieme lst)` → `3` |
| `(dernier l)` | Dernier élément | `(dernier lst)` → `30` |
| `(contient l x)` | Vrai si x est dans l | `(contient lst 2)` → `1` |
| `(somme_liste l)` | Somme des éléments | `(somme_liste lst)` → `60` |
| `(max_liste l)` | Maximum de la liste | `(max_liste lst)` → `30` |
| `(min_liste l)` | Minimum de la liste | `(min_liste lst)` → `10` |
| `(nth l n)` | Nième élément (base 0) | `(nth lst 1)` → `20` |

---

## Runtime

Le fichier `juliesp_runtime.h` est installé dans `/usr/local/include`. Il est automatiquement inclus dans tout fichier C généré et fournit le support pour les listes JULIESP via un système de valeurs taguées (`lisp_obj`).

Les listes peuvent contenir des **entiers** et des **chaînes de caractères**.

