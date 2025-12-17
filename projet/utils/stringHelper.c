#include "stringHelper.h"

char **splitCommandLine(const char *commandLine, int *wordCount)
{
    char **words = malloc(sizeof(char *) * COMMAND_MAX_LENGTH);
    if (!words)
        return NULL;

    int wordIndex = 0;
    const char *cursor = commandLine;

    while (*cursor)
    {
        while (*cursor == ' ' || *cursor == '\t' || *cursor == '\n')
            cursor++;
        if (*cursor == '\0')
            break;

        char buffer[COMMAND_MAX_LENGTH];
        int bufferIndex = 0;

        if (*cursor == '"')
        {
            cursor++;
            while (*cursor && *cursor != '"' && bufferIndex < COMMAND_MAX_LENGTH - 1)
                buffer[bufferIndex++] = *cursor++;
            if (*cursor == '"')
                cursor++;
        }
        else if ((cursor[0] == '&' && cursor[1] == '&') ||
                 (cursor[0] == '|' && cursor[1] == '|') ||
                 (cursor[0] == '>' && cursor[1] == '>') ||
                 (cursor[0] == '<' && cursor[1] == '<'))
        {
            buffer[bufferIndex++] = *cursor++;
            buffer[bufferIndex++] = *cursor++;
        }
        else if (*cursor == '>' || *cursor == '<' || *cursor == '|' || *cursor == '&')
        {
            buffer[bufferIndex++] = *cursor++;
        }
        else
        {
            while (*cursor && *cursor != ' ' && *cursor != '\t' && *cursor != '\n' && *cursor != '"' &&
                   !(cursor[0] == '&' && cursor[1] == '&') &&
                   !(cursor[0] == '|' && cursor[1] == '|') &&
                   !(cursor[0] == '>' && cursor[1] == '>') &&
                   !(cursor[0] == '<' && cursor[1] == '<') &&
                   *cursor != '>' && *cursor != '<' && *cursor != '|' && *cursor != '&' &&
                   bufferIndex < COMMAND_MAX_LENGTH - 1)
            {
                buffer[bufferIndex++] = *cursor++;
            }
        }

        buffer[bufferIndex] = '\0';
        if (bufferIndex > 0)
            words[wordIndex++] = strdup(buffer);
    }

    *wordCount = wordIndex;
    return words;
}

void freeWords(char **words, int count)
{
    for (int index = 0; index < count; index++)
        free(words[index]);
    free(words);
}