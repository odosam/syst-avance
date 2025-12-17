#pragma once

/**
 * @file command_reader.h
 * @brief Lecture et parsing des lignes de commande.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "command.h"
#include "../utils/stringHelper.h"
#include "env_vars/env_vars.h"
#include "alias/aliases.h"

#define MAX_COMMANDS_PER_LINE 10

/**
 * @brief Lit une ligne de commande depuis stdin.
 * @return Pointeur vers un buffer statique contenant la ligne, ou NULL en cas d'EOF.
 */
char *readCommandLine(void);

/**
 * @brief Parse une ligne de commande en tableau de commandes.
 * @param commandLine La ligne de commande à parser.
 * @param commandCount Pointeur vers le nombre de commandes parsées (sortie).
 * @return Tableau de commandes alloué dynamiquement, ou NULL si erreur.
 */
struct Command *parseCommand(char *commandLine, int *commandCount);

/**
 * @brief Retourne l'opérateur correspondant à un token.
 * @param token Le token à analyser.
 * @return L'opérateur correspondant, ou OP_NONE si ce n'est pas un opérateur.
 */
enum Operator getOperator(const char *token);
