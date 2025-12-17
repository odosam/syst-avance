#include "pathHelper.h"

char *getWorkingDirectory(void)
{
    char buffer[PATH_MAX_LENGTH];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
        return strdup(buffer);
    return NULL;
}

char *combinePath(const char *basePath, const char *relativePath)
{
    char temp[PATH_MAX_LENGTH];

    if (relativePath[0] == '/')
        strncpy(temp, relativePath, PATH_MAX_LENGTH);
    else
        snprintf(temp, PATH_MAX_LENGTH, "%s/%s", basePath, relativePath);

    char resolved[PATH_MAX_LENGTH];
    if (realpath(temp, resolved) == NULL)
        return NULL;

    return strdup(resolved);
}