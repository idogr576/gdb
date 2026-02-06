#pragma once
#include <sys/types.h>
#include <stdbool.h>
#include "symbols.h"
#include "breakpoint.h"

typedef struct tracee_state
{
    bool start;
    bool is_running;
} state;

typedef struct tracee
{
    pid_t pid;
    state state;
    symtab symtab;
    hash_t breakpoints;
} tracee;
