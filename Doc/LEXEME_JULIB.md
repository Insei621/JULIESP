# JULIESP — Référence rapide julib

Bibliothèque standard de JULIESP. Chargement :

```lisp
($ "lib/julib.jlsp")
```

---

## Arithmétique

| Fonction | Arguments | Retour | Description | Exemple |
|----------|-----------|--------|-------------|---------|
| `abs` | `n` | entier | Valeur absolue | `(abs -5)` → `5` |
| `max2` | `a b` | entier | Maximum de deux valeurs | `(max2 3 7)` → `7` |
| `min2` | `a b` | entier | Minimum de deux valeurs | `(min2 3 7)` → `3` |
| `carre` | `n` | entier | Carré de n | `(carre 4)` → `16` |
| `cube` | `n` | entier | Cube de n | `(cube 3)` → `27` |
| `pair` | `n` | 0 ou 1 | Vrai si n est pair | `(pair 4)` → `1` |
| `impair` | `n` | 0 ou 1 | Vrai si n est impair | `(impair 3)` → `1` |
| `clamp` | `val lo hi` | entier | Limite val entre lo et hi | `(clamp 15 0 10)` → `10` |
| `puissance` | `base exp` | entier | base^exp (entier) | `(puissance 2 8)` → `256` |
| `factorielle` | `n` | entier | n! récursif | `(factorielle 5)` → `120` |
| `fib` | `n` | entier | Fibonacci récursif | `(fib 7)` → `13` |
| `somme_n` | `n` | entier | Somme de 1 à n | `(somme_n 10)` → `55` |

---

## Logique

| Fonction | Arguments | Retour | Description | Exemple |
|----------|-----------|--------|-------------|---------|
| `non` | `b` | 0 ou 1 | Négation booléenne | `(non µ)` → `0` |
| `et` | `a b` | 0 ou 1 | ET logique | `(et µ ù)` → `0` |
| `ou` | `a b` | 0 ou 1 | OU logique | `(ou µ ù)` → `1` |
| `xou` | `a b` | 0 ou 1 | OU exclusif | `(xou µ µ)` → `0` |

---

## Listes

| Fonction | Arguments | Retour | Description | Exemple |
|----------|-----------|--------|-------------|---------|
| `longueur` | `l` | entier | Nombre d'éléments | `(longueur lst)` → `3` |
| `deuxieme` | `l` | valeur | 2ème élément | `(deuxieme lst)` → `2` |
| `troisieme` | `l` | valeur | 3ème élément | `(troisieme lst)` → `3` |
| `dernier` | `l` | valeur | Dernier élément | `(dernier lst)` → `30` |
| `contient` | `l x` | 0 ou 1 | Vrai si x est dans l | `(contient lst 2)` → `1` |
| `somme_liste` | `l` | entier | Somme des éléments | `(somme_liste lst)` → `60` |
| `max_liste` | `l` | entier | Maximum de la liste | `(max_liste lst)` → `30` |
| `min_liste` | `l` | entier | Minimum de la liste | `(min_liste lst)` → `10` |
| `nth` | `l n` | valeur | Nième élément (base 0) | `(nth lst 1)` → `20` |

---

## Exemple complet

```lisp
($ "lib/julib.jlsp")

§§ Arithmétique
(€ (factorielle 6))              §§ → 720
(€ (fib 10))                     §§ → 55
(€ (puissance 2 10))             §§ → 1024
(€ (clamp 150 0 100))            §§ → 100
(€ (pair 4))                     §§ → 1
(€ (impair 7))                   §§ → 1

§§ Logique
(€ (non µ))                      §§ → 0
(€ (et µ µ))                     §§ → 1
(€ (ou µ ù))                     §§ → 1
(€ (xou µ µ))                    §§ → 0

§§ Listes
(: lst (& 10 (& 20 (& 30 ²()))))
(€ (longueur lst))               §§ → 3
(€ (deuxieme lst))               §§ → 20
(€ (dernier lst))                §§ → 30
(€ (max_liste lst))              §§ → 30
(€ (somme_liste lst))            §§ → 60
(€ (contient lst 20))            §§ → 1
(€ (nth lst 2))                  §§ → 30
```
