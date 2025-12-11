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

int isBuiltinCommand(char *cmd){

    return (
        strcmp(cmd, "cd") == 0 ||
        strcmp(cmd, "pwd") == 0 ||
        strcmp(cmd, "echo") == 0 ||
        strcmp(cmd, "exit") == 0 
    );
}



int builtin_cd(char **args) {

    if (args[1] == NULL) {
        fprintf(stderr, "cd: missing argument\n");
        return 1;
    }
    if (chdir(args[1]) != 0) {
        perror("cd");
        return 1;
    }
    return 0;
}

int builtin_pwd() {
    char buffer[1024];

    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        printf("%s\n", buffer);
    } else {
        perror("pwd");
        return 1;
    }
    return 0;
}

int builtin_echo(char **args) {

    for (int i = 1; args[i] != NULL; i++){
        printf("%s", args[i]);
        if(args[i + 1] != NULL){
            printf(" ");
        }
    }
    printf("\n");
    return 0;
}

int builtin_exit(char **args) {
    exit(0);
    return 0; 
}

int runBuiltinCommand(char **args){

    if (strcmp(args[0], "cd") == 0) 
        return builtin_cd(args);
    else if (strcmp(args[0], "pwd") == 0)
        return builtin_pwd();
    else if (strcmp(args[0], "echo") == 0)
        return builtin_echo(args);
    else if (strcmp(args[0], "exit") == 0)
        exit(0);
    
    return -1; 
}

int isExportCommand(char *cmd){

    return (strcmp(cmd, "export") == 0);
}

int runExportCommand(char **args){

    if(args[1] == NULL){
        fprintf(stderr, "export: missing argument\n");
        return 1;
    }

    char *equalSign = strchr(args[1], '=');
    if(!equalSign){
        fprintf(stderr, "export: invalid format, use KEY=VALUE\n");
        return 1;
    }

    *equalSign = '\0';
    char *varName = args[1];
    char *varValue = equalSign + 1;

    if(setenv(varName, varValue, 1) != 0){
        perror("setenv");
        return 1;
    }
    else{
        printf("Exported: %s=%s\n", varName, varValue);
    }

    return 0;
}


