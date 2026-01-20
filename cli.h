#pragma once
#include "operation.h"

typedef struct command_op
{
    char *cmdline;
    void (*func_op)(state *, int, char *);
} command_op;

command_op read_command();