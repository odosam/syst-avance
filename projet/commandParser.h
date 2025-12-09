#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "typedef.h"

char *concat(char *str[], int start, int end);
char getOperator(char *str);
struct Command *parseCommands(char *args[], int argc, int *numCommands);
char isBackGroundOperator(char *args[], int argc);
void fillCommand(struct Command *cmd, char *args[], int start, int end);
