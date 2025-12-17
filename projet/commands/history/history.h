#pragma once

/**
 * @file history.h
 * @brief Gestion de l'historique des commandes.
 */

#include <stdio.h>
#include "../command.h"

#define HISTORY_FILE "historic.txt"

/**
 * @brief Ajoute une commande à l'historique.
 * @param cmd La commande à ajouter.
 */
void addToHistory(struct Command *cmd);

/**
 * @brief Affiche l'historique des commandes.
 */
void showHistory(void);
