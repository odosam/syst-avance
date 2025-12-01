#include "commandParser.h"

char *concat(char *args[], int start, int end)
{
    static char command[1024] = "";
    command[0] = '\0';

    for (int i = start; i < end; i++)
    {
        strcat(command, args[i]);
        if (i < end - 1)
        {
            strcat(command, " ");
        }
    }
    return command;
}

char getOperator(char *args)
{
    if (strcmp(args, "&&") == 0)
        return SUCCESS_ONLY;
    else if (strcmp(args, "||") == 0)
        return FAIL_ONLY;
    else if (strcmp(args, ">") == 0)
        return REDIR_OUT;
    else if (strcmp(args, "<") == 0)
        return REDIR_IN;
    else if (strcmp(args, ">>") == 0)
        return REDIR_APPEND;
    else if (strcmp(args, "<<") == 0)
        return REDIR_MULTILINE;
    else
        return NONE;
}

char isBackGroundOperator(char *args[], int argc)
{
    char *last = args[argc - 1];
    int len = strlen(last);
    return (len > 0 && last[len - 1] == '&');
}

void fillCommand(struct Command *cmd, char *args[], int start, int end)
{
    cmd->start = start;
    cmd->end = end;
    cmd->operatorType = NONE;
    cmd->redirType = NONE_REDIR;
    cmd->filename = NULL;

    int i = start;
    while (i < end)
    {
        char opType = getOperator(args[i]);
        if (opType == REDIR_OUT || opType == REDIR_IN || opType == REDIR_APPEND || opType == REDIR_MULTILINE)
        {
            if (i + 1 >= end)
                break;

            cmd->redirType = opType;
            cmd->filename = args[i + 1];
            i += 2;
            continue;
        }
        i++;
    }
}

struct Command *parseCommands(char *args[], int argc, int *numCommands)
{
    const int maxCommands = 100;
    struct Command *commands = malloc(sizeof(struct Command) * maxCommands);

    if (!commands)
        return NULL;

    int commandIndex = 0;
    int segmentStart = 1;

    for (int i = 1; i < argc && commandIndex < maxCommands; i++)
    {
        char opType = getOperator(args[i]);

        if (opType == SUCCESS_ONLY || opType == FAIL_ONLY)
        {
            if (segmentStart < i)
            {
                fillCommand(&commands[commandIndex], args, segmentStart, i);
                commands[commandIndex].operatorType = opType;
                commandIndex++;
            }
            segmentStart = i + 1;
        }
        else if (opType == REDIR_OUT || opType == REDIR_IN || opType == REDIR_APPEND || opType == REDIR_MULTILINE)
            i++;
    }

    if (segmentStart < argc && commandIndex < maxCommands)
    {
        fillCommand(&commands[commandIndex], args, segmentStart, argc);
        commandIndex++;
    }

    *numCommands = commandIndex;
    return commands;
}
