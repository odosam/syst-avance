#include "aliases.h"

static Alias aliases[MAX_ALIASES];
static int aliasCount = 0;

void setAlias(const char *name, char **words, int wordCount)
{
    char **expandedWords = NULL;
    int expandedCount = 0;

    for (int index = 0; index < wordCount; index++)
    {
        int parsedCount = 0;
        char **parsedWords = splitCommandLine(words[index], &parsedCount);

        expandedWords = realloc(expandedWords, sizeof(char *) * (expandedCount + parsedCount));
        for (int parsedIndex = 0; parsedIndex < parsedCount; parsedIndex++)
            expandedWords[expandedCount + parsedIndex] = parsedWords[parsedIndex];

        expandedCount += parsedCount;
        free(parsedWords);
    }

    Alias *alias = &aliases[aliasCount++];
    alias->name = strdup(name);
    alias->words = expandedWords;
    alias->wordCount = expandedCount;
}

char **substituteAlias(char **words, int *wordCount)
{
    for (int i = 0; i < aliasCount; i++)
    {
        if (strcmp(words[0], aliases[i].name) == 0)
        {
            int newCount = *wordCount - 1 + aliases[i].wordCount;
            char **newWords = malloc(sizeof(char *) * newCount);

            for (int j = 0; j < aliases[i].wordCount; j++)
                newWords[j] = strdup(aliases[i].words[j]);

            for (int j = 1; j < *wordCount; j++)
                newWords[aliases[i].wordCount + j - 1] = strdup(words[j]);

            freeWords(words, *wordCount);
            *wordCount = newCount;

            return substituteAlias(newWords, wordCount);
        }
    }
    return words;
}