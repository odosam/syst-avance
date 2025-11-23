#include "commandParser.h"

char *concat(char *str[], int start, int end)
{
    // tableau de caracteres pour stocker la commande
    static char command[1024] = "";
    command[0] = '\0';

    // boucle pour parcourir les arguments de la commande
    for (int i = start; i < end; i++)
    {
        // fonction strcat permet de concatener des chaines de caracteres
        strcat(command, str[i]);
        // ajouter un espace entre les arguments
        if (i < end - 1)
        {
            // ajouter un espace entre les arguments
            strcat(command, " ");
        }
    }
    return command;
}

char getOperator(char *str)
{
    // comparer la chaine de caracteres avec les operateurs && et ||
    if (strcmp(str, "&&") == 0)
    {
        return SUCCESS_ONLY;
    }
    else if (strcmp(str, "||") == 0)
    {
        return FAIL_ONLY;
    }
    // aucun operateur
    else
    {
        return NONE;
    }
}

// Fonction pour parser les commandes et les operateurs de controle
struct Command *parseCommands(char *args[], int argc, int *numCommands)
{
    // tableau pour stocker les commandes
    struct Command *commands = malloc(sizeof(struct Command) * 100);
    if (!commands)
        return NULL;
    int commandIndex = 0;
    int start = 1;

    // boucle pour parcourir les arguments
    for (int i = 1; i < argc; i++)
    {
        // obtenir le type d'operateur
        char operatorType = getOperator(args[i]);
        if (operatorType != NONE)
        {
            // stocker la commande precedente
            commands[commandIndex].start = start;
            commands[commandIndex].end = i;
            commands[commandIndex].operatorType = operatorType;
            // incrementer l'index des commandes
            commandIndex++;
            // mettre a jour le debut de la prochaine commande
            start = i + 1;
        }
    }

    // Ajouter la dernière commande
    if (start < argc)
    {
        // stocker la derniere commande
        commands[commandIndex].start = start;
        commands[commandIndex].end = argc;
        commands[commandIndex].operatorType = NONE;
        commandIndex++;
    }
    // retourner le nombre de commandes parsees
    *numCommands = commandIndex;
    // retourner le tableau des commandes
    return commands;
}