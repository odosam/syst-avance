#include "command_reader.h"

char *readCommandLine(void)
{
    printf("Prompt > ");
    static char buffer[COMMAND_MAX_LENGTH];
    if (fgets(buffer, COMMAND_MAX_LENGTH, stdin) != NULL)
        return buffer;
    return NULL;
}

enum Operator getOperator(const char *token)
{
    if (strcmp(token, "&&") == 0)
        return OP_AND;
    if (strcmp(token, "||") == 0)
        return OP_OR;
    if (strcmp(token, "&") == 0)
        return OP_BACKGROUND;
    if (strcmp(token, "|") == 0)
        return OP_PIPE;
    return OP_NONE;
}

static struct Command createCommand(char **words, int startIndex, int endIndex, enum Operator operator)
{
    struct Command command;
    int argumentCount = endIndex - startIndex;

    command.name = strdup(words[startIndex]);
    command.argCount = argumentCount - 1;
    command.args = malloc(sizeof(char *) * (argumentCount + 1));

    for (int index = 0; index < argumentCount; index++)
        command.args[index] = strdup(words[startIndex + index]);

    command.args[argumentCount] = NULL;
    command.op = operator;

    return command;
}

struct Command *parseCommand(char *commandLine, int *commandCount)
{
    int wordCount = 0;
    char **words = splitCommandLine(commandLine, &wordCount);
    if (words == NULL || wordCount == 0)
    {
        *commandCount = 0;
        return NULL;
    }

    words = substituteEnvVars(words, &wordCount);
    words = substituteAlias(words, &wordCount);

    struct Command *commands = malloc(sizeof(struct Command) * MAX_COMMANDS_PER_LINE);
    if (!commands)
    {
        freeWords(words, wordCount);
        *commandCount = 0;
        return NULL;
    }

    int commandIndex = 0;
    int firstWordIndex = 0;

    for (int currentIndex = 0; currentIndex < wordCount; currentIndex++)
    {
        enum Operator operator = getOperator(words[currentIndex]);
        if (operator != OP_NONE)
        {
            commands[commandIndex] = createCommand(words, firstWordIndex, currentIndex, operator);
            commandIndex++;
            firstWordIndex = currentIndex + 1;
        }
    }

    if (firstWordIndex < wordCount)
    {
        commands[commandIndex] = createCommand(words, firstWordIndex, wordCount, OP_NONE);
        *commandCount = commandIndex + 1;
    }
    else
    {
        *commandCount = commandIndex;
    }

    freeWords(words, wordCount);
    return commands;
}