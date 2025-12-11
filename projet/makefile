# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = bin/program

# Fichiers sources et objets
SRCS = main.c commandParser.c historic.c alias.c
OBJS = $(SRCS:%.c=bin/%.o)

# Règle par défaut
all: $(TARGET)

# Créer le répertoire bin s'il n'existe pas et compiler le programme
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compiler les fichiers .c en .o dans le dossier bin
bin/%.o: %.c | bin
	$(CC) $(CFLAGS) -c $< -o $@

doc:
	doxygen $(DOC_DIR)/doxygen.conf
	
# Créer le dossier bin
bin:
	mkdir bin

# Nettoyer les fichiers compilés
clean:
	rmdir /s /q bin 2>nul || true

# Recompiler entièrement
rebuild: clean all

# Exécuter le programme
run: $(TARGET)
	$(TARGET)

.PHONY: all clean rebuild run
