#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "external/logger.h"

#include "utils/path.h"

bool binary_path_exists(char *binary_path, size_t maxlen)
{
    bool path_exists = false;
    if (access(binary_path, F_OK) == -1)
    {
        LOG_DEBUG("did not find \"%s\", searching in PATH", binary_path);
        char search_binary_path[PATH_MAX_LEN] = {0};
        char *path_env = getenv("PATH");

        char *token = strtok(path_env, ":");

        while (token != NULL)
        {
            snprintf(search_binary_path, sizeof(search_binary_path), "%s/%s", token, binary_path);
            // check if the file exists and has execute permissions
            if (!access(search_binary_path, X_OK))
            {
                LOG_DEBUG("found binary in %s", search_binary_path);
                strncpy(binary_path, search_binary_path, maxlen);
                path_exists = true;
                break;
            }
            token = strtok(NULL, ":");
        }
    }
    else
    {
        LOG_DEBUG("found local binary path!");
        if (access(binary_path, X_OK) == -1)
        {
            LOG_ERROR("binary does not have execute permission!");
        }
        else
        {
            path_exists = true;
        }
    }
    LOG_DEBUG("binary_path = %s\n", binary_path);
    return path_exists;
}
