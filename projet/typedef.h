#pragma once
#include <stdio.h>

// Enum pour les types d'opérateurs de contrôle
enum CommandControlOperator
{
    NONE = 0,
    SUCCESS_ONLY = 10,
    FAIL_ONLY = 11
};

// Enum pour les types de redirections
enum RedirectionType
{
    NONE_REDIR = 0,
    REDIR_OUT = 20,      // >make
    REDIR_IN = 21,       // <
    REDIR_APPEND = 22,   // >>
    REDIR_MULTILINE = 23 // <<
};

// Structure pour stocker les informations d'une commandes
struct Command
{
    int start;
    int end;
    enum CommandControlOperator operatorType;
    enum RedirectionType redirType;
    char *filename;
};
