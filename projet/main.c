#include <stdio.h>
#include <stdlib.h>
#include "commandParser.h"

int main(int argc, char *argv[])
{
    // la fonction system permet d'executer une commande shell
    // system(concat(argv, 1, argc));

    /*
    Les opérateurs de contrôle : && et ||
     Les redirections de flux simples : |, >, <, >>, <<
     L’exécution en arrière-plan : &
    */

    // && : Exécute la deuxième commande seulement si la première réussit
    // || : Exécute la deuxième commande seulement si la première échoue

    int numCommands;
    struct Command *commands = parseCommands(argv, argc, &numCommands);
    printf("Nombre de commandes : %d\n", numCommands);
    for (int i = 0; i < numCommands; i++)
    {
        struct Command cmd = commands[i];
        system(concat(argv, cmd.start, cmd.end));
    }

    return 0;
}