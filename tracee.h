#pragma once
#include <unistd.h>
#include <stdbool.h>
#include "symbols.h"

typedef struct tracee_state {
    bool start;
    bool is_running;
} state;

typedef struct tracee  {
    pid_t pid;
    state state;
    symtab symtab;
} tracee;
