#pragma once
#include "operation.h"

#define MAX_PROMPT_SIZE 32

typedef struct command_op
{
    char *cmdline;
    void (*func_op)(tracee *, char *);
} command_op;

void cli_init();

command_op read_command(char *prefix);