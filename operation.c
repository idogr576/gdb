#include <stdio.h>
#include <sys/ptrace.h>
#include <logger.h>
#include <string.h>
#include <stdbool.h>

#include "operation.h"
#include "symbols.h"
#include "parser.h"
#include "registers.h"
#include "x86_64.h"
#include "breakpoint.h"

#define BUF_SIZE 1000
#define SEP_CHR ' '

void run_op(tracee *tracee, char *cmd)
{
    if (tracee->state.start)
    {
        return;
    }
    LOG_DEBUG("operarion RUN");
    puts("-----------------------------");
    puts("starting execution of program");
    puts("-----------------------------");
    tracee->state.start = true;
    tracee->state.is_running = true;
    ptrace(PTRACE_CONT, tracee->pid, 0, 0);
}

void continue_op(tracee *tracee, char *cmd)
{
    LOG_DEBUG("operation CONTINUE");
    if (!tracee->state.start)
    {
        puts("start execution with \"r\"");
        return;
    }
    tracee->state.is_running = true;
    ptrace(PTRACE_CONT, tracee->pid, 0, 0);
}

void next_op(tracee *tracee, char *cmd)
{
    LOG_DEBUG("operation NEXT");
    if (tracee->state.start && !tracee->state.is_running)
    {
        ptrace(PTRACE_SINGLESTEP, tracee->pid, 0, 0);
        tracee->state.is_running = true;
    }
}

void examine_op(tracee *tracee, char *cmd)
{
    LOG_DEBUG("operation EXAMINE");
}

void print_op(tracee *tracee, char *cmd)
{
    LOG_DEBUG("operation PRINT");
    char buf[BUF_SIZE] = {0};
    char fullfmt[BUF_SIZE] = {0};
    char fmt = 'x';
    // try to read the input
    if (sscanf(cmd, "p/%c %s", &fmt, buf) != 2)
    {
        sscanf(cmd, "p %s", buf);
    }
    if (!buf)
    {
        LOG_ERROR("cannot read cmd to buffer");
        return;
    }
    Value val = resolve_value(tracee, buf);

    if (fmt == 'd')
    {
        strcpy(fullfmt, "{ %s == %ld }\n");
    }
    else
    {
        strcpy(fullfmt, "{ %s == 0x%lx }\n");
    }
    printf(fullfmt, buf, val);
}

void breakpoint_op(tracee *tracee, char *cmd)
{
    LOG_DEBUG("operation BREAKPOINT");
    if (strlen(cmd) == 1)
    {
        breakpoint_list(tracee->breakpoints);
    }
    if (cmd[1] == ' ')
    {
        Value addr = resolve_value(tracee, &cmd[2]);
        if (!addr.addr)
        {
            puts("address does not exists");
        }
        else
        {
            breakpoint_set(&tracee->breakpoints, tracee->pid, addr.addr);
        }
    }
    if (cmd[1] == 'd')
    {
        Value addr = resolve_value(tracee, &cmd[3]);
        if (!addr.addr)
        {
            puts("address does not exists");
        }
        else
        {
            breakpoint_unset(&tracee->breakpoints, tracee->pid, addr.addr);
        }
    }
}

void help_op(tracee *tracee, char *cmd)
{
    char buffer[BUF_SIZE] = {0};
    FILE *helpfp = fopen("./docs/help.txt", "r");
    fread(buffer, sizeof(*buffer), BUF_SIZE, helpfp);
    puts(buffer);
}

void quit_op(tracee *tracee, char *cmd)
{
    puts("Goodby from gdb!");
}

void list_op(tracee *tracee, char *cmd)
{
    LOG_DEBUG("operation LIST");
}
