#pragma once
#include "operation.h"

typedef enum command
{
    UNDEFINED,
    RUN,
    CONTINUE,
    NEXT,
    EXAMINE,
    PRINT,
    BREAKPOINT,
    HELP,
    QUIT
} command;

typedef struct command_op
{
    char *cmdline;
    void (*func_op)(state *, int, char *);
} command_op;

command_op read_command();