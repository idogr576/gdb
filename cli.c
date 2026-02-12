#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <logger.h>

#include "cli.h"
#include "operation.h"

command_op read_command(char *prefix)
{
    size_t maxsize = 0;
    char *cmdline = NULL;
    char chr_op;
    command_op cmd_op = {NULL, NULL};

    rewind(stdin);
    printf("%s ", prefix);

    if (getline(&cmdline, &maxsize, stdin) == -1)
    {
        LOG_DEBUG("cannot read line from user");
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

    chr_op = tolower(cmdline[0]);

    switch (chr_op)
    {
    case 'r':
        cmd_op.func_op = run_op;
        break;

    case 'b':
        cmd_op.func_op = breakpoint_op;
        break;

    case 'c':
        cmd_op.func_op = continue_op;
        break;

    case 'n':
        cmd_op.func_op = next_op;
        break;

    case 'p':
        cmd_op.func_op = print_op;
        break;

    case 'x':
        cmd_op.func_op = examine_op;
        break;

    case 'h':
        cmd_op.func_op = help_op;
        break;

    case 'q':
        cmd_op.func_op = quit_op;
        break;

    case 'i':
        cmd_op.func_op = info_op;
        break;

    default:
        cmd_op.func_op = NULL;
    }

    return cmd_op;

error:
    // define error as quit op
    if (cmdline)
    {
        free(cmdline);
    }
    return (command_op){.cmdline = "q", .func_op = quit_op};
}
