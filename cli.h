#pragma once
#include "operation.h"

typedef struct command_op
{
    char *cmdline;
    void (*func_op)(tracee *, char *);
} command_op;

command_op read_command(char *prefix);