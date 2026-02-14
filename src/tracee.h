#pragma once
#include <sys/types.h>
#include <stdbool.h>
#include "elf/symbols.h"
#include "utils/hashmap.h"

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
