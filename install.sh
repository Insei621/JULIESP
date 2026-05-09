#!/bin/bash
# install.sh — Script d'installation de JulieSP

set -e  # Arrête si une commande échoue

echo ""
echo "  juliesp — Installation du compilateur JulieSP"
echo ""

# --- Vérifie les dépendances ---
echo "[1/5] Vérification des dépendances..."

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
echo "[2/5] Compilation..."

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release > /dev/null
make -j$(nproc)
cd ..

echo "  OK"

# --- Installation du binaire et du runtime ---
echo "[3/5] Installation dans /usr/local/bin..."

sudo make -C build install

echo "  OK"

# --- Installation de la page man ---
echo "[4/5] Installation de la page man..."

sudo mkdir -p /usr/share/man/man1
sudo cp juliesp.1 /usr/share/man/man1/juliesp.1
sudo gzip -f /usr/share/man/man1/juliesp.1
sudo mandb > /dev/null 2>&1

echo "  OK — man juliesp disponible"

# --- Vérifie l'installation ---
echo "[5/5] Vérification..."

if command -v juliesp &> /dev/null; then
    echo "  OK — juliesp installé avec succès !"
    echo ""
    echo "  Utilisation : juliesp <fichier.jlsp>"
    echo "  Options     : juliesp -h"
    echo "  Manuel      : man juliesp"
    echo ""
else
    echo "  ERREUR : juliesp n'est pas accessible dans le PATH."
    exit 1
fi
