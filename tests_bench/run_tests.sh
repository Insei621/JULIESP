#!/bin/bash
# run_tests.sh — Lance tous les tests unitaires JuliesSP
# Usage : ./run_tests.sh [chemin_vers_juliesp]

JULIESP="${1:-juliesp}"
TESTS_DIR="$(dirname "$0")"
PASS=0
FAIL=0
ERRORS=()

echo ""
echo "  JuliesSP — Suite de tests unitaires"
echo "  ====================================="
echo ""

for test_file in "$TESTS_DIR"/unit_*.jlsp; do
    name=$(basename "$test_file")

    # Compile le test
    "$JULIESP" "$test_file" -o "/tmp/juliesp_test_out.c" 2>/tmp/juliesp_err.txt

    if [ $? -ne 0 ]; then
        echo "  [FAIL] $name"
        echo "         Erreur de compilation :"
        cat /tmp/juliesp_err.txt | sed 's/^/         /'
        FAIL=$((FAIL + 1))
        ERRORS+=("$name")
        continue
    fi

    # Compile le C généré
    gcc /tmp/juliesp_test_out.c -o /tmp/juliesp_test_bin \
        -I/usr/local/include 2>/tmp/gcc_err.txt

    if [ $? -ne 0 ]; then
        echo "  [FAIL] $name"
        echo "         Erreur gcc :"
        cat /tmp/gcc_err.txt | sed 's/^/         /'
        FAIL=$((FAIL + 1))
        ERRORS+=("$name")
        continue
    fi

    # Exécute le binaire
    /tmp/juliesp_test_bin > /tmp/juliesp_test_result.txt 2>&1

    if [ $? -ne 0 ]; then
        echo "  [FAIL] $name — erreur à l'exécution"
        FAIL=$((FAIL + 1))
        ERRORS+=("$name")
    else
        echo "  [PASS] $name"
        PASS=$((PASS + 1))
    fi
done

echo ""
echo "  ====================================="
echo "  Résultats : $PASS passés, $FAIL échoués"

if [ ${#ERRORS[@]} -gt 0 ]; then
    echo ""
    echo "  Tests échoués :"
    for e in "${ERRORS[@]}"; do
        echo "    - $e"
    done
fi

echo ""
exit $FAIL
