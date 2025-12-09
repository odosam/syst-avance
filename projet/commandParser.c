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
    cmd->numRedirs = 0;
    cmd->pipeTo = -1;
    for (int k = 0; k < 4; k++)
    {
        cmd->redirType[k] = NONE_REDIR;
        cmd->filename[k] = NULL;
    }
    int i = start;
    while (i < end)
    {
        char opType = getOperator(args[i]);
        if (opType == REDIR_OUT || opType == REDIR_IN || opType == REDIR_APPEND || opType == REDIR_MULTILINE)
        {
            if (i + 1 >= end || cmd->numRedirs >= 4)
                break;
            cmd->redirType[cmd->numRedirs] = opType;
            cmd->filename[cmd->numRedirs] = args[i + 1];
            cmd->numRedirs++;
            i += 2;
            continue;
        }
        if (strcmp(args[i], "|") == 0)
        {
            cmd->pipeTo = 1;
            i++;
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
    int lastWasRedir = 0;
    for (int i = 1; i < argc && commandIndex < maxCommands; i++)
    {
        char opType = getOperator(args[i]);
        if (strcmp(args[i], "|") == 0)
        {
            if (segmentStart < i)
            {
                fillCommand(&commands[commandIndex], args, segmentStart, i);
                commands[commandIndex].pipeTo = 1;
                commandIndex++;
            }
            segmentStart = i + 1;
            lastWasRedir = 0;
            continue;
        }
        if (opType == SUCCESS_ONLY || opType == FAIL_ONLY)
        {
            if (segmentStart < i)
            {
                fillCommand(&commands[commandIndex], args, segmentStart, i);
                commands[commandIndex].operatorType = opType;
                commandIndex++;
            }
            segmentStart = i + 1;
            lastWasRedir = 0;
        }
        else if (opType == REDIR_OUT || opType == REDIR_IN || opType == REDIR_APPEND || opType == REDIR_MULTILINE)
        {
            lastWasRedir = 1;
            i++;
        }
        else
        {
            if (lastWasRedir && segmentStart < i)
            {
                fillCommand(&commands[commandIndex], args, segmentStart, i);
                commandIndex++;
                segmentStart = i;
            }
            lastWasRedir = 0;
        }
    }
    if (segmentStart < argc && commandIndex < maxCommands)
    {
        fillCommand(&commands[commandIndex], args, segmentStart, argc);
        commandIndex++;
    }

    *numCommands = commandIndex;
    return commands;
}
