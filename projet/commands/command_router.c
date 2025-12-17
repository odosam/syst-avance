#include "command_router.h"

static int cmdCd(struct Command *command, struct ProgramState *state);
static int cmdExit(struct Command *command, struct ProgramState *state);
static int cmdHistory(struct Command *command, struct ProgramState *state);
static int cmdPwd(struct Command *command, struct ProgramState *state);
static int cmdExport(struct Command *command, struct ProgramState *state);
static int cmdEcho(struct Command *command, struct ProgramState *state);
static int cmdAlias(struct Command *command, struct ProgramState *state);
static void applyRedirections(struct Command *command);

static NativeCommand nativeCommands[] = {
    {"cd", cmdCd},
    {"exit", cmdExit},
    {"history", cmdHistory},
    {"pwd", cmdPwd},
    {"export", cmdExport},
    {"echo", cmdEcho},
    {"alias", cmdAlias}};

#define NATIVE_COMMAND_COUNT (sizeof(nativeCommands) / sizeof(NativeCommand))

static int cmdCd(struct Command *command, struct ProgramState *state)
{
    if (command->argCount < 1)
    {
        printf("cd: argument manquant\n");
        return 1;
    }

    char *newPath = combinePath(state->workingDirectory, command->args[1]);
    if (newPath == NULL)
    {
        printf("cd: chemin invalide\n");
        return 1;
    }

    free(state->workingDirectory);
    state->workingDirectory = newPath;
    return 0;
}

static int cmdExit(struct Command *command, struct ProgramState *state)
{
    (void)command;
    state->isAlive = false;
    printf("Au revoir\n");
    return 0;
}

static int cmdHistory(struct Command *command, struct ProgramState *state)
{
    (void)command;
    (void)state;
    showHistory();
    return 0;
}

static int cmdPwd(struct Command *command, struct ProgramState *state)
{
    (void)command;
    printf("%s\n", state->workingDirectory);
    return 0;
}

static int cmdExport(struct Command *command, struct ProgramState *state)
{
    (void)state;
    if (command->argCount < 1)
    {
        printf("export: argument manquant\n");
        return 1;
    }

    char *argument = command->args[1];
    char *equalSign = strchr(argument, '=');
    if (equalSign == NULL)
    {
        printf("export: format invalide (utiliser VAR=valeur)\n");
        return 1;
    }

    *equalSign = '\0';
    setEnvVar(argument, equalSign + 1);
    return 0;
}

static int cmdEcho(struct Command *command, struct ProgramState *state)
{
    (void)state;
    for (int index = 1; command->args[index] != NULL; index++)
    {
        if (index > 1)
            printf(" ");
        printf("%s", command->args[index]);
    }
    printf("\n");
    return 0;
}

static int cmdAlias(struct Command *command, struct ProgramState *state)
{
    (void)state;
    if (command->argCount < 2)
    {
        printf("alias: utilisation: alias nom commande [args...]\n");
        return 1;
    }
    setAlias(command->args[1], &command->args[2], command->argCount - 1);
    return 0;
}

static int openOutputFile(const char *filename, int appendMode)
{
    int flags = O_WRONLY | O_CREAT | (appendMode ? O_APPEND : O_TRUNC);
    int fileDescriptor = open(filename, flags, 0644);
    if (fileDescriptor < 0)
    {
        perror("open");
        _exit(1);
    }
    dup2(fileDescriptor, STDOUT_FILENO);
    close(fileDescriptor);
    return 1;
}

static int openInputFile(const char *filename)
{
    int fileDescriptor = open(filename, O_RDONLY);
    if (fileDescriptor < 0)
    {
        perror("open");
        _exit(1);
    }
    dup2(fileDescriptor, STDIN_FILENO);
    close(fileDescriptor);
    return 1;
}

static int handleHereDocument(const char *delimiter)
{
    int pipeDescriptors[2];
    if (pipe(pipeDescriptors) < 0)
    {
        perror("pipe");
        _exit(1);
    }

    char lineBuffer[1024];
    printf("> ");
    fflush(stdout);

    while (fgets(lineBuffer, sizeof(lineBuffer), stdin) != NULL)
    {
        size_t length = strlen(lineBuffer);
        if (length > 0 && lineBuffer[length - 1] == '\n')
            lineBuffer[length - 1] = '\0';

        if (strcmp(lineBuffer, delimiter) == 0)
            break;

        strcat(lineBuffer, "\n");
        write(pipeDescriptors[1], lineBuffer, strlen(lineBuffer));
        printf("> ");
        fflush(stdout);
    }

    close(pipeDescriptors[1]);
    dup2(pipeDescriptors[0], STDIN_FILENO);
    close(pipeDescriptors[0]);
    return 1;
}

static void applyRedirections(struct Command *command)
{
    if (command->args == NULL)
        return;

    for (int index = 1; command->args[index] != NULL; index++)
    {
        const char *token = command->args[index];
        const char *targetFile = command->args[index + 1];

        if (strcmp(token, ">") == 0 && targetFile)
        {
            openOutputFile(targetFile, 0);
            command->args[index] = NULL;
            return;
        }

        if (strcmp(token, ">>") == 0 && targetFile)
        {
            openOutputFile(targetFile, 1);
            command->args[index] = NULL;
            return;
        }

        if (strcmp(token, "<") == 0 && targetFile)
        {
            openInputFile(targetFile);
            command->args[index] = NULL;
            return;
        }

        if (strcmp(token, "<<") == 0 && targetFile)
        {
            handleHereDocument(targetFile);
            command->args[index] = NULL;
            return;
        }
    }
}

int executeCommand(struct Command *command, struct ProgramState *state)
{
    for (size_t index = 0; index < NATIVE_COMMAND_COUNT; index++)
    {
        if (strcmp(command->name, nativeCommands[index].name) == 0)
            return nativeCommands[index].fn(command, state);
    }

    pid_t processId = fork();
    if (processId == 0)
    {
        applyRedirections(command);
        execvp(command->name, command->args);
        perror("execvp");
        _exit(127);
    }

    if (processId < 0)
    {
        perror("fork");
        return -1;
    }

    if (command->op == OP_BACKGROUND)
    {
        printf("[%d] démarré en arrière-plan\n", processId);
        return 0;
    }

    int status;
    waitpid(processId, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

int executePipeline(struct Command *commands, int commandCount, struct ProgramState *state)
{
    if (commandCount == 1)
        return executeCommand(&commands[0], state);

    int previousPipe[2] = {-1, -1};
    pid_t *processIds = malloc(sizeof(pid_t) * commandCount);
    if (!processIds)
        return -1;

    for (int index = 0; index < commandCount; index++)
    {
        int currentPipe[2] = {-1, -1};

        if (index < commandCount - 1 && pipe(currentPipe) < 0)
        {
            perror("pipe");
            free(processIds);
            return -1;
        }

        pid_t processId = fork();
        if (processId == 0)
        {
            if (previousPipe[0] != -1)
            {
                dup2(previousPipe[0], STDIN_FILENO);
                close(previousPipe[0]);
                close(previousPipe[1]);
            }

            if (currentPipe[1] != -1)
            {
                dup2(currentPipe[1], STDOUT_FILENO);
                close(currentPipe[0]);
                close(currentPipe[1]);
            }

            applyRedirections(&commands[index]);
            execvp(commands[index].name, commands[index].args);
            perror("execvp");
            _exit(127);
        }

        if (processId < 0)
        {
            perror("fork");
            free(processIds);
            return -1;
        }

        processIds[index] = processId;

        if (previousPipe[0] != -1)
        {
            close(previousPipe[0]);
            close(previousPipe[1]);
        }

        previousPipe[0] = currentPipe[0];
        previousPipe[1] = currentPipe[1];
    }

    if (previousPipe[0] != -1)
    {
        close(previousPipe[0]);
        close(previousPipe[1]);
    }

    int lastStatus = 0;
    for (int index = 0; index < commandCount; index++)
    {
        int status;
        waitpid(processIds[index], &status, 0);
        if (index == commandCount - 1)
            lastStatus = WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }

    free(processIds);
    return lastStatus;
}