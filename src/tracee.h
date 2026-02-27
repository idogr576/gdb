#pragma once
#include <sys/types.h>
#include <stdbool.h>
#include "elf/symbols.h"

typedef struct tracee_state
{
    bool start;
    bool is_running;
} state;

typedef struct { GElf_Addr key; char value; } * hm_t;

typedef struct tracee
{
    pid_t pid;
    state state;
    symtab symtab;
    hm_t breakpoints;
} tracee;
