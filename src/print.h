#pragma once

#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_RESET "\x1b[0m"

#define PRINT printf

#define RED(X) COLOR_RED X COLOR_RESET
#define GREEN(X) COLOR_GREEN X COLOR_RESET
#define YELLOW(X) COLOR_YELLOW X COLOR_RESET
#define BLUE(X) COLOR_BLUE X COLOR_RESET
#define MAGENTA(X) COLOR_MAGENTA X COLOR_RESET
#define CYAN(X) COLOR_CYAN X COLOR_RESET
