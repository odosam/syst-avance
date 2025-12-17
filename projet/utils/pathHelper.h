#pragma once

/**
 * @file pathHelper.h
 * @brief Fonctions utilitaires pour la gestion des chemins.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PATH_MAX_LENGTH 2048

/**
 * @brief Retourne le répertoire de travail actuel.
 * @return Chemin alloué dynamiquement, ou NULL si erreur.
 */
char *getWorkingDirectory(void);

/**
 * @brief Combine un chemin de base avec un chemin relatif.
 * @param basePath Le chemin de base.
 * @param relativePath Le chemin relatif à combiner.
 * @return Chemin absolu résolu alloué dynamiquement, ou NULL si invalide.
 */
char *combinePath(const char *basePath, const char *relativePath);