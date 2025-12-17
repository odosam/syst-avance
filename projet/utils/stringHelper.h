#pragma once

/**
 * @file stringHelper.h
 * @brief Fonctions utilitaires pour la manipulation de chaînes.
 */

#include <string.h>
#include <stdlib.h>

#define COMMAND_MAX_LENGTH 1024

/**
 * @brief Découpe une ligne de commande en tableau de mots.
 * @param commandLine La ligne de commande à découper.
 * @param wordCount Pointeur vers le nombre de mots (sortie).
 * @return Tableau de mots alloué dynamiquement.
 */
char **splitCommandLine(const char *commandLine, int *wordCount);

/**
 * @brief Libère un tableau de mots.
 * @param words Le tableau de mots à libérer.
 * @param count Le nombre de mots dans le tableau.
 */
void freeWords(char **words, int count);