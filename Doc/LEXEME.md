# JulieSP — Table des lexèmes

Référence rapide de tous les tokens du langage JulieSP.

---

## Ignorés

| Token | Symbole | Description |
|-------|---------|-------------|
| `DEL_SPACE` | ` ` `\t` `\n` | Espaces et retours à la ligne |
| `COM_LINE` | `§§ ...` | Commentaire sur une ligne |
| `COM_BLOCK` | `§! ... !§` | Commentaire multi-lignes |

---

## Littéraux

| Token | Symbole | Exemple | Description |
|-------|---------|---------|-------------|
| `LIT_INT` | entier | `42`, `-3` | Entier signé |
| `LIT_FLOAT` | flottant | `3.14` | Flottant (séparateur `.`) |
| `LIT_STRING` | chaîne | `"hello"` | Chaîne de caractères |
| `LIT_CHAR` | caractère | `'a'` | Caractère unique |
| `BOOL_TRUE` | `µ` | `µ` | Vrai (T) |
| `BOOL_FALSE` | `ù` | `ù` | Faux / NIL |

---

## Délimiteurs

| Token | Symbole | Description |
|-------|---------|-------------|
| `DEL_LBRACE` | `(` | Parenthèse ouvrante |
| `DEL_RBRACE` | `)` | Parenthèse fermante |

---

## Formes spéciales

| Token | Symbole | Nb args | Description |
|-------|---------|---------|-------------|
| `CORE_SETQ` | `:` | 2 | Assignation — `(: nom valeur)` |
| `CORE_IF` | `?` | 2 ou 3 | Condition — `(? cond then else?)` |
| `CORE_LAMBDA` | `£` | 2 | Fonction — `(£ (params) corps)` |
| `CORE_PROGN` | `;` | n | Séquence — `(; expr1 expr2 ...)` |
| `CORE_PRINT` | `€` | 1 | Affichage — `(€ expr)` |
| `CORE_SCAN` | `ç` | 1 | Saisie int — `(ç var)` |
| `CORE_LOAD` | `$` | 1 | Chargement — `($ "fichier.jlsp")` |
| `CORE_QUOTE` | `²` | 1 | Citation — `(² expr)` |

---

## Primitives de listes

| Token | Symbole | Nb args | Description |
|-------|---------|---------|-------------|
| `MAIN_CAR` | `<<` | 1 | Premier élément — `(<< lst)` |
| `MAIN_CDR` | `>>` | 1 | Reste — `(>> lst)` |
| `MAIN_CONS` | `&` | 2 | Construction — `(& val lst)` |
| `MAIN_NULL` | `\|` | 1 | Liste vide ? — `(\| lst)` |
| `MAIN_ATOM` | `@` | 1 | Atome ? — `(@ val)` |

---

## Opérateurs arithmétiques

| Token | Symbole | Nb args | Description |
|-------|---------|---------|-------------|
| `CALC_PLUS` | `+` | n | Addition |
| `CALC_MOINS` | `-` | 2 | Soustraction |
| `CALC_MULT` | `*` | n | Multiplication |
| `CALC_DIV` | `/` | 2 | Division entière |
| `CALC_INF` | `<` | 2 | Inférieur à |
| `CALC_SUP` | `>` | 2 | Supérieur à |
| `CALC_EQ` | `=` | 2 | Égalité |
| `CALC_ADREQ` | `¤` | 2 | Identité physique |
| `CALC_NUMBERQ` | `°` | 1 | Est un nombre ? |

---

## Identifiants

| Token | Exemple | Regex |
|-------|---------|-------|
| `IDENT` | `ma_var`, `foo123` | `^[A-Za-z_][A-Za-z0-9_]*` |

> Les tirets `-` ne sont pas autorisés dans les noms de variables. Utiliser `_` à la place.

---

## Rappel syntaxe

```lisp
§§ Assignation
(: x 42)

§§ Condition
(? (> x 0) (€ "positif") (€ "négatif"))

§§ Fonction
(: double (£ (n) (* n 2)))

§§ Liste
(: lst (& 1 (& 2 (& 3 ²()))))

§§ Chargement de la librairie standard
($ "lib/julib.jlsp")
```
