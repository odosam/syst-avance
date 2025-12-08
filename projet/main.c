#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "commandParser.h"
#include "historic.h"

int main(int argc, char *argv[])
{
    int numCommands;

    // Parse les commandes et les différents opérateurs
    struct Command *commands = parseCommands(argv, argc, &numCommands);
    if (!commands)
    {
        fprintf(stderr, "Erreur : échec du parsing des commandes.\n");
        return 1;
    }

    // Check la présence de l'opérateur d'exécution en arrière-plan
    char isBg = isBackGroundOperator(argv, argc);

    // Execute les commandes parsées
    for (int i = 0; i < numCommands; i++)
    {
        struct Command cmd = commands[i];
        saveCommand(concat(argv, cmd.start, cmd.end));
        if (strcmp(argv[cmd.start], "history") == 0)
        {
            displayHistoric();
            continue;
        }

        // Création d'un processus fils
        pid_t pid = fork();

        // Si le processus fils fonctionne
        if (pid == 0)
        {
            // 1 - Préparation des arguments de la commande à exécuter ---------------------------
            int maxArgs = (cmd.end > cmd.start) ? (cmd.end - cmd.start) : 0;
            char *cmd_argv[maxArgs + 1];
            int size = 0;

            int argIndex = cmd.start;
            while (argIndex < cmd.end)
            {
                char *token = argv[argIndex];
                char opType = getOperator(token);

                if (opType == REDIR_OUT || opType == REDIR_IN || opType == REDIR_APPEND || opType == REDIR_MULTILINE)
                {
                    if (argIndex + 1 >= cmd.end)
                    {
                        fprintf(stderr, "Erreur : nom de fichier manquant pour la redirection.\n");
                        exit(1);
                    }
                    argIndex += 2; // saute l'opérateur et son fichier
                    continue;
                }

                cmd_argv[size++] = token;
                argIndex++;
            }

            cmd_argv[size] = NULL;

            if (size == 0)
                exit(1);

            // On récupère le dernier argument pour verifier la presence de '&'
            char *last = cmd_argv[size - 1];
            int len = strlen(last);

            // si le dernier caractere est '&'
            if (len > 0 && last[len - 1] == '&')
            {
                if (len == 1)
                {
                    // On remplace le '&' du dernier argument par NULL
                    cmd_argv[--size] = NULL;
                }
                else
                    // le dernier caractere du dernier élement du tableau
                    last[len - 1] = '\0';
            }

            // 3 - Gestion des redirections --------------------------------------------------
            if (cmd.redirType != NONE_REDIR && cmd.filename != NULL)
            {
                int fd; // Desripteur de fichier pour la redirection
                switch (cmd.redirType)
                {
                case REDIR_OUT:                                                  //  >
                    fd = open(cmd.filename, O_WRONLY | O_CREAT | O_TRUNC, 0644); // ouverture du fichier en écriture seule | crée si n'existe pas | vide s'il existe
                                                                                 // 0644 = permissions rw-r--r--
                    if (fd < 0)
                    {
                        perror("open");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO); // redirige la sortie standard vers le fichier
                    close(fd);               // ferme le descripteur de fichier
                    break;
                case REDIR_IN:                         //  <
                    fd = open(cmd.filename, O_RDONLY); // ouverture du fichier en lecture seule
                    if (fd < 0)
                    {
                        perror("open");
                        exit(1);
                    }
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                    break;
                case REDIR_APPEND:                                                //  >>
                    fd = open(cmd.filename, O_WRONLY | O_CREAT | O_APPEND, 0644); // ouverture du fichier en écriture seule | crée si n'existe pas | ajoute à la fin s'il existe
                    if (fd < 0)
                    {
                        perror("open");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    break;
                // case REDIR_MULTILINE: //  <<
                //     break;
                default:
                    break;
                }
            }

            // déplace le flux d'exécution vers la commande
            execvp(cmd_argv[0], cmd_argv);
            // si execvp échoue
            perror("execvp");
            exit(1);
        }
        else if (pid > 0)
        {
            if (!isBg)
                // attendre la fin du processus avant d'envoyer la prochaine commande
                waitpid(pid, NULL, 0);
        }
        else
        {
            perror("fork");
            free(commands);
            return 1;
        }
    }

    // libérer la mémoire allouée
    free(commands);
    return 0;
}