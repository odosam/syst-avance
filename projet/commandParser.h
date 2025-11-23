#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "typedef.h"

// Fonction pour concatener les arguments de la commande
char *concat(char *str[], int start, int end);
// Fonction pour obtenir le type d'opérateur de contrôle
char getOperator(char *str);
// Fonction pour parser les commandes et les operateurs de controle
struct Command *parseCommands(char *args[], int argc, int *numCommands);
