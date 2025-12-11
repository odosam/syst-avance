#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "commandParser.h"
#include "historic.h"
#include "alias.h"

int main(int argc, char *argv[])
{
    int numCommands;
    int argCount = 0;
    char *args[1024];
    for (int i = 0; i < argc; i++, argCount++)
    {
        char *str = argv[i];
        int len = strlen(str);
        for (int j = 0; j < len; j++)
        {
            if (str[j] == ' ')
            {
                str[j] = '\0';
                args[argCount++] = str;
                str += (j + 1);
                j = -1;
            }
        }

        args[i] = str;
    }

    struct Command *commands = parseCommands(args, argc, &numCommands);
    if (!commands)
    {
        return 1;
    }

    char isBg = isBackGroundOperator(argv, argc);

    for (int i = 0; i < numCommands; i++)
    {
        struct Command cmd = commands[i];
        int maxArgs = (cmd.end > cmd.start) ? (cmd.end - cmd.start) : 0;
        char *cmd_argv[maxArgs + 1];

        int size = 0;
        int argIndex = cmd.start;

        while (argIndex < cmd.end)
        {
            char *token = argv[argIndex];
            char opType = getOperator(token);
            if (opType == REDIR_OUT || opType == REDIR_IN || opType == REDIR_APPEND || opType == REDIR_MULTILINE || strcmp(token, "|") == 0)
            {
                argIndex++;
                if (opType != NONE && argIndex < cmd.end)
                    argIndex++;
                continue;
            }

            cmd_argv[size++] = token;
            argIndex++;
        }

        cmd_argv[size] = NULL;

        char *aliasValue = getAliasValue(cmd_argv[0]);
        if (aliasValue != NULL) {
            // Décomposer l'alias en plusieurs tokens
            char *tokens[1024];
            int k = 0;
            char *temp = strdup(aliasValue);  // copie de l'alias
            char *token = strtok(temp, " ");
            while (token != NULL && k < 1023) {
                tokens[k++] = token;
                token = strtok(NULL, " ");
            }
            tokens[k] = NULL;

            for (int j = 0; j < k; j++)
                cmd_argv[j] = tokens[j];
            cmd_argv[k] = NULL;
            size = k;

        }

        // Si c'est la commande alias elle-même
        if (strcmp(cmd_argv[0], "alias") == 0) {
            if (size == 1) {
                listAliases(); 
            } else {
                char *eq = strchr(cmd_argv[1], '=');
                if (eq != NULL) {
                    *eq = '\0';
                    addAlias(cmd_argv[1], eq + 1);
                } else {
                    fprintf(stderr, "alias: invalid format\n");
                }
            }
            continue; 
        }
 


        saveCommand(concat(argv, cmd.start, cmd.end));

        if (strcmp(argv[cmd.start], "history") == 0)
        {
            displayHistoric();
            continue;
        }

        if(size > 0 && isExportCommand(cmd_argv[0])){
           runExportCommand(cmd_argv);
           continue;
        }


        if(size > 0 && isBuiltinCommand(cmd_argv[0])){
           runBuiltinCommand(cmd_argv);
           continue;
        }

        

        int pipefd[2];
        int hasPipe = (cmd.pipeTo == 1 && i < numCommands - 1);

        if (hasPipe)
        {
            pipe(pipefd);
        }

        pid_t pid = fork();

        if (pid == 0)
        {
            
            

            if (size == 0)
                exit(1);

            char *last = cmd_argv[size - 1];
            int len = strlen(last);

            if (len > 0 && last[len - 1] == '&')
            {
                if (len == 1)
                    cmd_argv[--size] = NULL;
                else
                    last[len - 1] = '\0';
            }

            if (strcmp(cmd_argv[0], "history") == 0)
            {
                displayHistoric();
                exit(0);
            }

            for (int r = 0; r < cmd.numRedirs; r++)
            {
                int fd;

                switch (cmd.redirType[r])
                {
                case REDIR_OUT:
                    fd = open(cmd.filename[r], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0)
                    {
                        perror("open");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    break;
                case REDIR_IN:
                    fd = open(cmd.filename[r], O_RDONLY);
                    if (fd < 0)
                    {
                        perror("open");
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                    break;
                case REDIR_APPEND:
                    fd = open(cmd.filename[r], O_WRONLY | O_CREAT | O_APPEND, 0644);
                    if (fd < 0)
                    {
                        perror("open");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    break;
                case REDIR_MULTILINE:
                {
                    int pipefd_heredoc[2];
                    pipe(pipefd_heredoc);
                    pid_t heredoc_pid = fork();
                    if (heredoc_pid == 0)
                    {
                        close(pipefd_heredoc[0]);
                        char buffer[1024];
                        while (1)
                        {
                            printf("> ");
                            fflush(stdout);
                            if (!fgets(buffer, sizeof(buffer), stdin))
                                break;
                            if (strncmp(buffer, cmd.filename[r], strlen(cmd.filename[r])) == 0 && buffer[strlen(cmd.filename[r])] == '\n')
                                break;
                            write(pipefd_heredoc[1], buffer, strlen(buffer));
                        }
                        close(pipefd_heredoc[1]);
                        exit(0);
                    }
                    else
                    {
                        close(pipefd_heredoc[1]);
                        dup2(pipefd_heredoc[0], STDIN_FILENO);
                        close(pipefd_heredoc[0]);
                        waitpid(heredoc_pid, NULL, 0);
                    }
                    break;
                }
                default:
                    break;
                }
            }

            if (hasPipe)
            {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }
            execvp(cmd_argv[0], cmd_argv);
            perror("execvp");
            exit(1);
        }
        else if (pid > 0)
        {
            {
                close(pipefd[1]);
                int nextPid = fork();
                if (nextPid == 0)
                {
                    dup2(pipefd[0], STDIN_FILENO);
                    close(pipefd[0]);
                    struct Command nextCmd = commands[i + 1];
                    int maxArgs2 = (nextCmd.end > nextCmd.start) ? (nextCmd.end - nextCmd.start) : 0;
                    char *cmd_argv2[maxArgs2 + 1];
                    int size2 = 0;
                    int argIndex2 = nextCmd.start;
                    while (argIndex2 < nextCmd.end)
                    {
                        char *token2 = argv[argIndex2];
                        char opType2 = getOperator(token2);
                        if (opType2 == REDIR_OUT || opType2 == REDIR_IN || opType2 == REDIR_APPEND || opType2 == REDIR_MULTILINE || strcmp(token2, "|") == 0)
                        {
                            argIndex2++;
                            if (opType2 != NONE && argIndex2 < nextCmd.end)
                                argIndex2++;
                            continue;
                        }
                        cmd_argv2[size2++] = token2;
                        argIndex2++;
                    }
                    cmd_argv2[size2] = NULL;
                    for (int r2 = 0; r2 < nextCmd.numRedirs; r2++)
                    {
                        int fd2;
                        switch (nextCmd.redirType[r2])
                        {
                        case REDIR_OUT:
                            fd2 = open(nextCmd.filename[r2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                            if (fd2 < 0)
                            {
                                perror("open");
                                exit(1);
                            }
                            dup2(fd2, STDOUT_FILENO);
                            close(fd2);
                            break;
                        case REDIR_IN:
                            fd2 = open(nextCmd.filename[r2], O_RDONLY);
                            if (fd2 < 0)
                            {
                                perror("open");
                                exit(1);
                            }
                            dup2(fd2, STDIN_FILENO);
                            close(fd2);
                            break;
                        case REDIR_APPEND:
                            fd2 = open(nextCmd.filename[r2], O_WRONLY | O_CREAT | O_APPEND, 0644);
                            if (fd2 < 0)
                            {
                                perror("open");
                                exit(1);
                            }
                            dup2(fd2, STDOUT_FILENO);
                            close(fd2);
                            break;
                        case REDIR_MULTILINE:
                            // heredoc à implémenter plus tard
                            break;
                        default:
                            break;
                        }
                    }
                    execvp(cmd_argv2[0], cmd_argv2);
                    perror("execvp");
                    exit(1);
                }
                else if (nextPid > 0)
                {
                    close(pipefd[0]);
                    waitpid(nextPid, NULL, 0);
                    i++; // saute la commande suivante car déjà exécutée
                }
            }
            if (!isBg)
                waitpid(pid, NULL, 0);
        }
        else
        {
            perror("fork");
            free(commands);
            return 1;
        }
    }
}