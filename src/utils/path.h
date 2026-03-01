#pragma once
#include <stdbool.h>

#define PATH_MAX_LEN 1024

// search if binary exists both locally and in PATH (in this case the binary_path is updated)
bool binary_path_exists(char *binary_path);
