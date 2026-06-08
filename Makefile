# ============================================================
#  Makefile  —  Compile tous les fichiers en un seul programme
# ============================================================
#
#  Commandes disponibles :
#    make        →  compiler le projet
#    make clean  →  supprimer les fichiers compilés
#
#  Après compilation, lancer avec :
#    sudo ./ping google.com
# ============================================================

# Le compilateur à utiliser
CC = gcc

# Les options de compilation :
#   -Wall   = afficher tous les avertissements
#   -O2     = optimiser le code
CFLAGS = -Wall -O2

# La bibliothèque mathématique (nécessaire pour sqrt)
LIBS = -lm

# Tous les fichiers source .c du projet
SOURCES = main.c \
          controle_envoi.c \
          ICMP.c \
          reseau.c \
          format_ip.c \
          affichage.c \
          calcul.c

# Les fichiers .o générés lors de la compilation
OBJECTS = $(SOURCES:.c=.o)

# Nom du programme final
TARGET = myping

# ── Règle principale : construire le programme ──
all: $(TARGET)
	@echo ""
	@echo "  Compilation reussie !"
	@echo "  Lancer avec : sudo ./$(TARGET) google.com"
	@echo ""

# Lier tous les .o pour créer l'exécutable final
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

# Compiler chaque .c en .o
# Si commun.h change, recompiler tous les fichiers
%.o: %.c commun.h
	$(CC) $(CFLAGS) -c $< -o $@

# ── Règle de nettoyage ──
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "Fichiers supprimes."

.PHONY: all clean
