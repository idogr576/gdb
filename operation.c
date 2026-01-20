#include <stdio.h>
#include <sys/ptrace.h>
#include <logger.h>
#include <string.h>

#include "operation.h"
#include "symbols.h"

#define BUF_SIZE 1000
#define SEP_CHR ' '

// define the global symbol table for gdb
symtab g_symtab;

void run_op(state *state, int pid, char *cmd)
{
    LOG_DEBUG("operarion RUN");
    puts("-----------------------------");
    puts("starting execution of program");
    puts("-----------------------------");
    state->start = true;
    state->is_running = true;
    ptrace(PTRACE_CONT, pid, 0, 0);
}

void continue_op(state *state, int pid, char *cmd)
{
    LOG_DEBUG("operation CONTINUE");
    if (!state->start)
    {
        puts("start execution with \"r\"");
        return;
    }
    state->is_running = true;
    ptrace(PTRACE_CONT, pid, 0, 0);
}

void next_op(state *state, int pid, char *cmd)
{
    LOG_DEBUG("operation NEXT");
}

void examine_op(state *state, int pid, char *cmd)
{
    LOG_DEBUG("operation EXAMINE");
}

void print_op(state *state, int pid, char *cmd)
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
    GElf_Sym *sym = symtab_find_sym(&g_symtab, ++sep);
    if (!sym)
    {
        printf("%s address not found!\n", sep);
    }
    else
    {
        GElf_Addr sym_addr = symtab_get_dyn_sym_addr(pid, sym);
        printf("%s == 0x%lx\n", sep, sym_addr);
    }
}

void breakpoint_op(state *state, int pid, char *cmd)
{
    LOG_DEBUG("operation BREAKPOINT");
    LOG_DEBUG("pid is %d, cmd is \"%s\"", pid, cmd);
}

void help_op(state *state, int pid, char *cmd)
{
    char buffer[BUF_SIZE] = {0};
    FILE *helpfp = fopen("./docs/help.txt", "r");
    fread(buffer, sizeof(*buffer), BUF_SIZE, helpfp);
    puts(buffer);
}

void quit_op(state *state, int pid, char *cmd)
{
    puts("Goodby from gdb!");
}

void list_op(state *state, int pid, char *cmd)
{
    LOG_DEBUG("operation LIST");
}