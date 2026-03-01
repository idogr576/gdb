#pragma once
#include "operation.h"
#include "tracee.h"

#define MAX_PROMPT_SIZE 128

typedef struct command_op
{
    char *cmdline;
    void (*func_op)(tracee *, char *);
} command_op;

void cli_init();

char *get_last_command();

command_op read_command(tracee *tracee, char *prefix);