#include "history.h"

void addToHistory(struct Command *cmd)
{
    FILE *file = fopen(HISTORY_FILE, "a");
    if (file == NULL)
    {
        perror("Erreur ouverture historique");
        return;
    }

    fprintf(file, "%s", cmd->name);
    for (int i = 1; cmd->args[i] != NULL; i++)
        fprintf(file, " %s", cmd->args[i]);
    fprintf(file, "\n");

    fclose(file);
}

void showHistory(void)
{
    FILE *file = fopen(HISTORY_FILE, "r");
    if (file == NULL)
    {
        perror("Erreur ouverture historique");
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL)
        printf("%s", line);

    fclose(file);
}
