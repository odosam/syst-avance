#include <stdio.h>
#include <string.h>
#include "alias.h"

struct Alias aliasTable[MAX_ALIASES];
int aliasCount = 0;

int addAlias(const char *name, const char *value)
{
    if (aliasCount >= MAX_ALIASES)
        return -1; 

    strncpy(aliasTable[aliasCount].name, name, MAX_ALIAS_NAME_LENGTH - 1);
    aliasTable[aliasCount].name[MAX_ALIAS_NAME_LENGTH - 1] = '\0';

    strncpy(aliasTable[aliasCount].value, value, MAX_ALIAS_VALUE_LENGTH -1);
    aliasTable[aliasCount].value[MAX_ALIAS_VALUE_LENGTH - 1] = '\0';

    aliasCount++;
    return 0; 
}

char const* getAliasValue(const char *name)
{
    for (int i = 0; i < aliasCount; i++)
    {
        if (strcmp(aliasTable[i].name, name) == 0)
        {
            return aliasTable[i].value;
        }
    }
    return NULL; 
}

void listAliases()
{
    for (int i = 0; i < aliasCount; i++)
    {
        printf("alias %s='%s'\n", aliasTable[i].name, aliasTable[i].value);
    }
}
