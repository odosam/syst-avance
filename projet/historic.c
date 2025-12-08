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
    while (fgets(command, 1024, file) != 0)
    {
        printf("%s", command);
    }
    fclose(file);
}
