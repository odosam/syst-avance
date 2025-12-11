#ifndef ALIAS_H
#define ALIAS_H

#define MAX_ALIAS_NAME_LENGTH 64
#define MAX_ALIAS_VALUE_LENGTH 256
#define MAX_ALIASES 100

struct Alias {
    char name[MAX_ALIAS_NAME_LENGTH];
    char value[MAX_ALIAS_VALUE_LENGTH];
};

extern struct Alias aliasTable[MAX_ALIASES];
extern int aliasCount;

int addAlias(const char *name, const char *value);
const char* getAliasValue(const char *name);
void listAliases();

#endif 
