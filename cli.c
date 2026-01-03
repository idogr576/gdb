#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <logger.h>

#include "cli.h"
#include "operation.h"

command read_command()
{
    size_t maxsize = 0;
    char *cmdline = NULL;
    char chr_op;
    command op;
    void (*func_op)(char *) = NULL;

    if (getline(&cmdline, &maxsize, stdin) == -1)
    {
        LOG_ERROR("cannot read line from user");
        return UNDEFINED;
    }

    LOG_DEBUG("read: \"%s\"", cmdline);
    chr_op = cmdline[0];

    switch (chr_op)
    {
    case 'R':
    case 'r':
        op = RUN;
        func_op = run_op;
        break;

    case 'B':
    case 'b':
        op = BREAKPOINT;
        func_op = breakpoint_op;
        break;

    case 'C':
    case 'c':
        op = CONTINUE;
        func_op = continue_op;
        break;

    case 'N':
    case 'n':
        op = NEXT;
        func_op = next_op;
        break;

    case 'P':
    case 'p':
        op = PRINT;
        func_op = print_op;
        break;

    case 'X':
    case 'x':
        op = EXAMINE;
        func_op = examine_op;
        break;

    default:
        op = UNDEFINED;
    }

    if (func_op)
    {
        func_op(cmdline);
    }
    return op;
}
