#pragma once

/**
 * @file programState.h
 * @brief État global du programme shell.
 */

#include <stdbool.h>

/**
 * @struct ProgramState
 * @brief État global du programme shell.
 */
struct ProgramState
{
    char *workingDirectory;
    bool isAlive;
};