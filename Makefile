# Makefile — Wrapper de compilation pour JulieSP

.PHONY: all install clean uninstall

# =============================================================================
# Compilation
# =============================================================================

all:
	@echo ""
	@echo "  juliesp — Compilation..."
	@echo ""
	@mkdir -p build
	@cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local > /dev/null
	@$(MAKE) -C build -j$$(nproc) --no-print-directory
	@echo ""
	@echo "  OK — Compilation terminée."
	@echo "  Lancez 'sudo make install' pour installer juliesp."
	@echo ""

# =============================================================================
# Installation
# =============================================================================

install: all
	@echo ""
	@echo "  juliesp — Installation..."
	@echo ""
	@sudo $(MAKE) -C build install --no-print-directory
	@sudo mandb > /dev/null 2>&1
	@echo ""
	@echo "  OK — juliesp installé avec succès !"
	@echo ""
	@echo "  Utilisation : juliesp <fichier.jlsp>"
	@echo "  Options     : juliesp -h"
	@echo "  Manuel      : man juliesp"
	@echo ""

# =============================================================================
# Nettoyage
# =============================================================================

clean:
	@echo "  Nettoyage..."
	@rm -rf build
	@echo "  OK"

# =============================================================================
# Désinstallation
# =============================================================================

uninstall:
	@echo "  Désinstallation..."
	@sudo rm -f /usr/local/bin/juliesp
	@sudo rm -f /usr/local/include/juliesp_runtime.h
	@sudo rm -f /usr/share/man/man1/juliesp.1.gz
	@sudo mandb > /dev/null 2>&1
	@echo "  OK — juliesp désinstallé."
