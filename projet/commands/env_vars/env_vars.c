#include "env_vars.h"

static EnvVar envVars[MAX_ENV_VARS];
static int envVarCount = 0;

void setEnvVar(const char *name, const char *value)
{
    for (int i = 0; i < envVarCount; i++)
    {
        if (strcmp(envVars[i].name, name) == 0)
        {
            free(envVars[i].value);
            envVars[i].value = strdup(value);
            return;
        }
    }

    envVars[envVarCount].name = strdup(name);
    envVars[envVarCount].value = strdup(value);
    envVarCount++;
}

static const char *getEnvVar(const char *name)
{
    for (int i = 0; i < envVarCount; i++)
    {
        if (strcmp(envVars[i].name, name) == 0)
            return envVars[i].value;
    }
    return NULL;
}

char **substituteEnvVars(char **words, int *wordCount)
{
    char **result = malloc(sizeof(char *) * (*wordCount * 10));
    int resultCount = 0;

    for (int i = 0; i < *wordCount; i++)
    {
        const char *word = words[i];

        if (word[0] == '$')
        {
            const char *varName = word + 1;
            const char *value = getEnvVar(varName);

            if (value && strlen(value) > 0)
            {
                int valueWordCount = 0;
                char **valueWords = splitCommandLine(value, &valueWordCount);
                for (int j = 0; j < valueWordCount; j++)
                    result[resultCount++] = valueWords[j];
                free(valueWords);
            }
            else
            {
                result[resultCount++] = strdup("");
            }
        }
        else
        {
            result[resultCount++] = strdup(word);
        }
    }

    *wordCount = resultCount;
    return result;
}