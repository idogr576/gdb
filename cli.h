#pragma once

#define CONSOLE printf

typedef enum command
{
    UNDEFINED,
    RUN,
    CONTINUE,
    NEXT,
    EXAMINE,
    PRINT,
    BREAKPOINT
} command;

typedef struct command_op
{
    char *cmdline;
    void (*func_op)(int, char *);
} command_op;

command_op read_command();