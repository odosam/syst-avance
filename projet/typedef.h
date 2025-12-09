#pragma once
#include <stdio.h>

enum CommandControlOperator
{
    NONE = 0,
    SUCCESS_ONLY = 10,
    FAIL_ONLY = 11
};
enum RedirectionType
{
    NONE_REDIR = 0,
    REDIR_OUT = 20,      // >make
    REDIR_IN = 21,       // <
    REDIR_APPEND = 22,   // >>
    REDIR_MULTILINE = 23 // <<
};
struct Command
{
    int start;
    int end;
    enum CommandControlOperator operatorType;
    enum RedirectionType redirType[4]; // supporte jusqu'à 4 redirections
    char *filename[4];
    int numRedirs;
    int pipeTo; // index de la commande suivante si pipe, -1 sinon
};
