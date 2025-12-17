#pragma once

/**
 * @file env_vars.h
 * @brief Gestion des variables d'environnement.
 */

#include <stdlib.h>
#include <string.h>
#include "../../utils/stringHelper.h"

#define MAX_ENV_VARS 100

/**
 * @struct EnvVar
 * @brief Structure représentant une variable d'environnement.
 */
typedef struct
{
    char *name;
    char *value;
} EnvVar;

/**
 * @brief Définit une variable d'environnement.
 * @param name Le nom de la variable.
 * @param value La valeur de la variable.
 */
void setEnvVar(const char *name, const char *value);

/**
 * @brief Substitue les variables d'environnement dans un tableau de mots.
 * @param words Le tableau de mots à traiter.
 * @param wordCount Pointeur vers le nombre de mots (entrée/sortie).
 * @return Nouveau tableau de mots.
 */
char **substituteEnvVars(char **words, int *wordCount);