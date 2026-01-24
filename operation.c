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
    /*
    cmd example: "p main"
    currently, the only supported object type is symbol
    */
    char *sep = strchr(cmd, SEP_CHR);
    if (!sep)
    {
        printf("invalid command \"%s\", missing ' ' after p\n", cmd);
        return;
    }
    ValueType type = identify_addr_type(++sep, &tracee->symtab);
    // check if register or address
    if (type == TYPE_REGISTER)
    {
        reg_t reg = get_register_value(tracee, sep + 1);
        printf("{ %s = %lld = 0x%llx }\n", sep, reg, reg);
    }
    else if (type == TYPE_ADDRESS || type == TYPE_SYMBOL)
    {
        GElf_Addr addr = resolve_address(type, tracee->pid, &tracee->symtab, sep);
        if (addr == (GElf_Addr)-1)
        {

            printf("cannot resolve address %s\n", sep);
        }
        else
        {
            printf("{ %s = 0x%lx }\n", sep, addr);
        }
    }
    else
    {
        puts("unknown value type");
    }
}

void breakpoint_op(tracee *tracee, char *cmd)
{
    LOG_DEBUG("operation BREAKPOINT");
    LOG_DEBUG("pid is %d, cmd is \"%s\"", tracee->pid, cmd);
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
