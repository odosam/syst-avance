#include "historic.h"

void saveCommand(char *command)
{
    FILE *file = fopen("historic.txt", "a+");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }
    fprintf(file, "%s\n", command);
    fclose(file);
}

void displayHistoric()
{
    FILE *file = fopen("historic.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    char command[1024];
    while (1)
    {
        char *res = fgets(command, 1024, file);
        if (res == NULL)
        {
            break;
        }
        printf("%s", command);
        fflush(stdout);
    }
    fclose(file);
    fflush(stdout);
}
