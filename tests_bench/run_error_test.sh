#!/bin/bash
JULIESP="${1:-juliesp}"
TESTS_DIR="$(dirname "$0")"
PASS=0
FAIL=0

echo ""
echo "  JULIESP — Tests des messages d'erreur"
echo "  ======================================="
echo ""

for test_file in "$TESTS_DIR"/err_*.jlsp; do
    name=$(basename "$test_file")
    "$JULIESP" "$test_file" -o "/tmp/juliesp_err_out.c" > /tmp/out.txt 2>&1
    if [ $? -ne 0 ]; then
        echo "  [PASS] $name"
        PASS=$((PASS + 1))
    else
        echo "  [FAIL] $name — le compilateur aurait dû échouer"
        FAIL=$((FAIL + 1))
    fi
done

# Tests de commande
echo "(: x 42)" > /tmp/test.jlsp

for test_cmd in \
    "err_cmd_01|Aucun fichier source|$JULIESP" \
    "err_cmd_02|Extension incorrecte|$JULIESP /tmp/test.c" \
    "err_cmd_03|Option inconnue|$JULIESP /tmp/test.jlsp --inconnu" \
    "err_cmd_04|-o sans argument|$JULIESP /tmp/test.jlsp -o" \
    "err_cmd_05|-b sans argument|$JULIESP /tmp/test.jlsp -b"
do
    id=$(echo "$test_cmd" | cut -d'|' -f1)
    desc=$(echo "$test_cmd" | cut -d'|' -f2)
    cmd=$(echo "$test_cmd" | cut -d'|' -f3)
    eval "$cmd" > /tmp/out.txt 2>&1
    if [ $? -ne 0 ]; then
        echo "  [PASS] $id — $desc"
        PASS=$((PASS + 1))
    else
        echo "  [FAIL] $id — $desc"
        FAIL=$((FAIL + 1))
    fi
done

echo ""
echo "  ======================================="
echo "  Résultats : $PASS passés, $FAIL échoués"
echo ""
exit $FAIL