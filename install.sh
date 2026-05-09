#!/bin/bash
# install.sh — Script d'installation de JuliesSP

set -e  # Arrête si une commande échoue

echo ""
echo "  juliesp — Installation du compilateur JuliesSP"
echo ""

# --- Vérifie les dépendances ---
echo "[1/4] Vérification des dépendances..."

if ! command -v cmake &> /dev/null; then
    echo "  ERREUR : cmake n'est pas installé."
    echo "  Installez-le avec : sudo apt install cmake"
    exit 1
fi

if ! command -v g++ &> /dev/null; then
    echo "  ERREUR : g++ n'est pas installé."
    echo "  Installez-le avec : sudo apt install g++"
    exit 1
fi

echo "  OK"

# --- Compilation ---
echo "[2/4] Compilation..."

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release > /dev/null
make -j$(nproc)
cd ..

echo "  OK"

# --- Installation ---
echo "[3/4] Installation dans /usr/local/bin..."

sudo make -C build install

echo "  OK"

# --- Vérifie l'installation ---
echo "[4/4] Vérification..."

if command -v juliesp &> /dev/null; then
    echo "  OK — juliesp installé avec succès !"
    echo ""
    echo "  Utilisation : juliesp <fichier.jlsp>"
    echo "  Aide        : juliesp --help"
    echo ""
else
    echo "  ERREUR : juliesp n'est pas accessible dans le PATH."
    exit 1
fi
