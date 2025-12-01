#include "commandParser.h"

char *concat(char *args[], int start, int end)
{
    // Tableau de caractères permettant de stocker la commande
    static char command[1024] = "";
    command[0] = '\0';

    // Boucle pour parcourir les arguments de la commande
    for (int i = start; i < end; i++)
    {
        // Appel à la fonction strcat permettant de concatener des chaînes de caractères
        strcat(command, args[i]);

        // Ajout d'un espace entre les arguments
        if (i < end - 1)
        {
            strcat(command, " ");
        }
    }
    return command;
}

// Fonction permettant d'obtenir le type d'opérateur entre les commandes
char  getOperator(char *args)
{
    // Comparaison de la chaîne de caractères avec les différents opérateurs possibles
    if (strcmp(args, "&&") == 0)
    {
        return SUCCESS_ONLY;
    }
    else if (strcmp(args, "||") == 0)
    {
        return FAIL_ONLY;
    }
    else if (strcmp(args, ">") == 0)
    {
        return REDIR_OUT;
    }
    else if ( strcmp(args, "<") == 0)
    {
        return REDIR_IN;
    }
    else if (strcmp(args, ">>") == 0)
    {
        return REDIR_APPEND;
    }
    else if (strcmp(args, "<<") == 0)
    {
        return REDIR_MULTILINE;
    }
    // aucun operateur
    else
    {
        return NONE;
    }
}

// Fonction pour savoir si la commande doit s'executer en arrière-plan ( présence de '&' dans le dernier argument )
char isBackGroundOperator(char *args[], int argc)
{
    char *last = args[argc - 1];    // Obtention du dernier argument
    int len = strlen(last);         // Calcul de sa longueur

    return (len > 0 && last[len - 1] == '&');
}

// Fonction permettant de parser les différentes commandes en fonction des operateurs détectés
struct Command *parseCommands(char *args[], int argc, int *numCommands)
{
    // Tableau qui stocke l'ensemble des commandes
    struct Command *commands = malloc(sizeof(struct Command) * 100);

    if (!commands)
        return NULL;

    int commandIndex = 0; // Indique à quelle position on est dans le tableau des commandes
    int start = 1;

    // Boucle pour parcourir les arguments et les séparer en commandes
    for (int i = 1; i < argc; i++)
    {
        // Obtient le type d'operateur
        char operatorType = getOperator(args[i]);

        if(operatorType == SUCCESS_ONLY || operatorType == FAIL_ONLY){
            // On stocke la commande en cours 
            commands[commandIndex].start = start;
            commands[commandIndex].end = i;
            commands[commandIndex].operatorType = operatorType;
            commands[commandIndex].redirType = NONE_REDIR;
            commands[commandIndex].filename = NULL;

            // Puis on incrémente l'index des commandes pour passer à la prochaine
            commandIndex++; 
            start = i + 1;  // La prochaine commande commence apres l'operateur
        }
        else if(operatorType == REDIR_OUT || operatorType == REDIR_IN || operatorType == REDIR_APPEND || operatorType == REDIR_MULTILINE){
            
            // On stocke le type de redirection et le nom du fichier
            commands[commandIndex].redirType = operatorType;

            if(i + 1 < argc){
                commands[commandIndex].filename = args[i + 1];
                commands[commandIndex].end = i;
            }
            else{
                fprintf(stderr,"Erreur : Nom de fichier manquant pour la redirection.\n");
                commands[commandIndex].redirType = NONE_REDIR;
                commands[commandIndex].filename = NULL;
            }
            
            
        }
        
        // // Si un operateur est détecté
        // if (operatorType != NONE)
        // {
        //     // On stocke la commande en cours 
        //     commands[commandIndex].start = start;
        //     commands[commandIndex].end = i;
        //     commands[commandIndex].operatorType = operatorType;

        //     // Puis on incrémente l'index des commandes pour passer à la prochaine
        //     commandIndex++; 
        //     start = i + 1;  // La prochaine commande commence apres l'operateur
        // }
    }

    // Ajoute la dernière commande (car elle n'est pas suivie d'un operateur) 
    if (start < argc)
    {
        // stocker la derniere commande
        commands[commandIndex].start = start;
        commands[commandIndex].end = argc;
        commands[commandIndex].operatorType = NONE;
        commands[commandIndex].redirType = NONE_REDIR;
        commands[commandIndex].filename = NULL;
        commandIndex++;
    }

    
    *numCommands = commandIndex;    // Nombre total de commandes parsees
    return commands;                // Retourne le tableau des commandes
}