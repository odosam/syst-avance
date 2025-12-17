#pragma once

/**
 * @file command_router.h
 * @brief Exécution des commandes natives et externes.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../state/programState.h"
#include "command.h"
#include "../utils/pathHelper.h"
#include "history/history.h"
#include "alias/aliases.h"
#include "env_vars/env_vars.h"

/**
 * @brief Type de fonction pour une commande native.
 */
typedef int (*NativeCommandFn)(struct Command *, struct ProgramState *);

/**
 * @struct NativeCommand
 * @brief Entrée dans la table des commandes natives.
 */
typedef struct
{
    const char *name;
    NativeCommandFn fn;
} NativeCommand;

/**
 * @brief Exécute une commande (native ou externe).
 * @param command La commande à exécuter.
 * @param state L'état du programme.
 * @return Le code de retour de la commande.
 */
int executeCommand(struct Command *command, struct ProgramState *state);

/**
 * @brief Exécute un pipeline de commandes.
 * @param commands Tableau de commandes du pipeline.
 * @param commandCount Nombre de commandes dans le pipeline.
 * @param state L'état du programme.
 * @return Le code de retour de la dernière commande.
 */
int executePipeline(struct Command *commands, int commandCount, struct ProgramState *state);