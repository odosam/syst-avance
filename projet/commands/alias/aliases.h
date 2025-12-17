#pragma once

/**
 * @file aliases.h
 * @brief Gestion des alias de commandes.
 */

#include <stdlib.h>
#include <string.h>
#include "../../utils/stringHelper.h"

#define MAX_ALIASES 100

/**
 * @struct Alias
 * @brief Structure représentant un alias de commande.
 */
typedef struct
{
    char *name;
    char **words;
    int wordCount;
} Alias;

/**
 * @brief Enregistre un nouvel alias.
 * @param name Le nom de l'alias.
 * @param words Les mots de remplacement.
 * @param wordCount Le nombre de mots.
 */
void setAlias(const char *name, char **words, int wordCount);

/**
 * @brief Substitue les alias dans un tableau de mots.
 * @param words Le tableau de mots à traiter.
 * @param wordCount Pointeur vers le nombre de mots (entrée/sortie).
 * @return Nouveau tableau de mots (l'ancien est libéré).
 */
char **substituteAlias(char **words, int *wordCount);
