#include <stdio.h>
#include <stdlib.h>
#include "commandParser.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    /*
     Les redirections de flux simples : |, >, <, >>, <<
    */
    int numCommands;
    // parser les commandes et les operateurs de controle
    struct Command *commands = parseCommands(argv, argc, &numCommands);
    // vérifier la présence de l'opérateur d'exécution en arrière-plan
    char isBg = isBackGroundOperator(argv, argc);

    for (int i = 0; i < numCommands; i++)
    {
        struct Command cmd = commands[i];
        // création d'un processus fils
        pid_t pid = fork();
        // si le processus fils fonctionne
        if (pid == 0)
        {
            // definir la taille du tableau de commande (nombre d'arguments)
            int size = cmd.end - cmd.start;
            // tableau pour stocker les arguments de la commande (+1 pour NULL)
            char *cmd_argv[size + 1];
            // remplir le tableau des arguments de la commande
            for (int j = 0; j < size; j++)
            {
                // copier les arguments de la commande
                cmd_argv[j] = argv[cmd.start + j];
            }

            // recuperer le dernier argument pour verifier la presence de '&'
            char *last = cmd_argv[size - 1];
            // la taille du dernier argument
            int len = strlen(last);
            // si le dernier caractere est '&'
            if (len > 0 && last[len - 1] == '&')
            {
                if (len == 1)
                {
                    // supprimer le '&' du dernier argument par NULL
                    cmd_argv[--size] = NULL;
                }
                else
                    // le dernier caractere du dernier élement du tableau
                    last[len - 1] = '\0';
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