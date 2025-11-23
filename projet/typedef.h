#pragma once
#include <stdio.h>

// Enum pour les types d'opérateurs de contrôle
enum CommandControlOperator
{
    NONE,
    SUCCESS_ONLY,
    FAIL_ONLY
};

// Structure pour stocker les informations d'une commandes
struct Command
{
    int start;
    int end;
    enum CommandControlOperator operatorType;
};
