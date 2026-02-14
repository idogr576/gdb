#pragma once
#include <sys/types.h>
#include <stdbool.h>
#include "symbols.h"
#include "hashmap.h"

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
    hm_t breakpoints;
} tracee;
