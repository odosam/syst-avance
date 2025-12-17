#pragma once

/**
 * @file command.h
 * @brief Définitions des structures de commandes du shell.
 */

/**
 * @enum Operator
 * @brief Opérateurs de séparation de commandes.
 */
enum Operator
{
    OP_NONE,
    OP_AND,
    OP_OR,
    OP_BACKGROUND,
    OP_PIPE
};

/**
 * @struct Command
 * @brief Structure représentant une commande parsée.
 */
struct Command
{
    char *name;
    char **args;
    int argCount;
    enum Operator op;
};