#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <logger.h>

#include "cli.h"
#include "operation.h"

command_op read_command()
{
    size_t maxsize = 0;
    char *cmdline = NULL;
    char chr_op;
    command_op cmd_op = {NULL, NULL};

    printf(">> ");

    if (getline(&cmdline, &maxsize, stdin) == -1)
    {
        LOG_ERROR("cannot read line from user");
        goto error;
    }

    // remove trailing '\n' from cmdline
    char *p = strchr(cmdline, '\n');
    if (p)
    {
        *p = '\0';
    }

    LOG_DEBUG("read: \"%s\"", cmdline);
    cmd_op.cmdline = cmdline;

    chr_op = cmdline[0];

    switch (chr_op)
    {
    case 'R':
    case 'r':
        cmd_op.func_op = run_op;
        break;

    case 'B':
    case 'b':
        cmd_op.func_op = breakpoint_op;
        break;

    case 'C':
    case 'c':
        cmd_op.func_op = continue_op;
        break;

    case 'N':
    case 'n':
        cmd_op.func_op = next_op;
        break;

    case 'P':
    case 'p':
        cmd_op.func_op = print_op;
        break;

    case 'X':
    case 'x':
        cmd_op.func_op = examine_op;
        break;

    case 'H':
    case 'h':
        cmd_op.func_op = help_op;
        break;

    case 'Q':
    case 'q':
        cmd_op.func_op = quit_op;
        break;

    default:
        cmd_op.func_op = NULL;
    }

    return cmd_op;

error:
    free(cmdline);
    return (command_op){NULL, NULL};
}
