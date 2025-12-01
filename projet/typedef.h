#pragma once
#include <stdio.h>

// Enum pour les types d'opérateurs de contrôle
enum CommandControlOperator
{
    NONE,
    SUCCESS_ONLY,
    FAIL_ONLY
};

// Enum pour les types de redirections
enum RedirectionType
{
    NONE_REDIR,
    REDIR_OUT,          // >
    REDIR_IN,           // <            
    REDIR_APPEND,       // >>
    REDIR_MULTILINE     // <<
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
