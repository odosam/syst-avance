#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "state/programState.h"
#include "utils/pathHelper.h"
#include "commands/command.h"
#include "commands/command_reader.h"
#include "commands/command_router.h"
#include "commands/history/history.h"

static struct ProgramState programState;

static int executeCommandLine(char *inputLine)
{
    int commandCount = 0;
    struct Command *commands = parseCommand(inputLine, &commandCount);
    if (commandCount == 0)
    {
        free(commands);
        return 0;
    }

    addToHistory(&commands[0]);

    int pipelineStartIndex = 0;
    int lastExitCode = 0;

    for (int index = 0; index < commandCount; index++)
    {
        if (commands[index].op == OP_PIPE)
            continue;

        int pipelineLength = index - pipelineStartIndex + 1;
        if (pipelineLength > 1)
            lastExitCode = executePipeline(&commands[pipelineStartIndex], pipelineLength, &programState);
        else
            lastExitCode = executeCommand(&commands[index], &programState);

        if (!programState.isAlive)
        {
            free(commands);
            return lastExitCode;
        }

        if (commands[index].op == OP_AND && lastExitCode != 0)
            break;
        if (commands[index].op == OP_OR && lastExitCode == 0)
            break;

        pipelineStartIndex = index + 1;
    }

    free(commands);
    return lastExitCode;
}

int main(int argc, char *argv[])
{
    programState.isAlive = true;
    programState.workingDirectory = getWorkingDirectory();
    if (programState.workingDirectory == NULL)
    {
        fprintf(stderr, "Erreur: impossible d'obtenir le répertoire courant\n");
        return 1;
    }

    if (argc >= 3 && strcmp(argv[1], "-c") == 0)
    {
        return executeCommandLine(argv[2]);
    }

    char *inputLine;
    while ((inputLine = readCommandLine()) != NULL)
    {
        executeCommandLine(inputLine);
        if (!programState.isAlive)
            return 0;
    }

    return 0;
}