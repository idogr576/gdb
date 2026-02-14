#include <stdio.h>
#include <sys/ptrace.h>
#include <logger.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>

#include "operation.h"
#include "symbols.h"
#include "parser.h"
#include "registers.h"
#include "x86_64.h"
#include "breakpoint.h"
#include "data_utils.h"
#include "print.h"

void run_op(tracee *tracee, char *cmd)
{
    if (tracee->state.start)
    {
        return;
    }
    LOG_DEBUG("operarion RUN");
    PRINT(YELLOW("-----------------------------\n"));
    PRINT(YELLOW("starting execution of program\n"));
    PRINT(YELLOW("-----------------------------")"\n");
    tracee->state.start = true;
    tracee->state.is_running = true;
    ptrace(PTRACE_CONT, tracee->pid, 0, 0);
}

void continue_op(tracee *tracee, char *cmd)
{
    LOG_DEBUG("operation CONTINUE");
    if (!tracee->state.start)
    {
        PRINT(RED("start execution with \"r\"\n"));
        return;
    }
    tracee->state.is_running = true;
    breakpoint_step(tracee);
    ptrace(PTRACE_CONT, tracee->pid, 0, 0);
}

void next_op(tracee *tracee, char *cmd)
{
    LOG_DEBUG("operation STEP");
    if (tracee->state.start && !tracee->state.is_running)
    {
        breakpoint_step(tracee);
        ptrace(PTRACE_SINGLESTEP, tracee->pid, 0, 0);
        tracee->state.is_running = true;
    }
}

void examine_op(tracee *tracee, char *cmd)
{
    LOG_DEBUG("operation EXAMINE");
    char buf[BUFSIZ] = {0};
    char fullfmt[BUFSIZ] = {0};
    char fmt = 'x';
    int n = 1;
    // try to read the input
    if (sscanf(cmd, "x/%d%c %s", &n, &fmt, buf) != 3 || !buf)
    {
        LOG_ERROR("cannot read cmd to buffer");
        return;
    }
    Value val = resolve_value(tracee, buf);

    if (fmt == 'i')
    {
        x86_64_disassemble(tracee, val.addr, n);
        return;
    }
    if (!strchr("xdbcs", fmt))
    {
        return;
    }
    uint8_t data[BUFSIZ] = {0};
    size_t size = read_tracee_mem(tracee, val.addr, data, n);
    LOG_DEBUG("read %ld bytes", size);
    if (fmt == 's')
    {
        PRINT(BLUE("%016" PRIX64) " %s\n", val.addr, data);
        return;
    }
    bool print_warning = true;
    for (int i = 0; i < n; i++)
    {
        if (fmt == 'x')
        {
            // TODO: does not display right data
            if(print_warning) PRINT(RED("WARNING: this formatting method is not accurate\n"));
            uint32_t *p = data;
            PRINT(BLUE("%016" PRIX64) " 0x%08lx\n", val.addr + i * sizeof(*p), p[i]);
        }
        if (fmt == 'd')
        {
            // TODO: does not display right data
            if(print_warning) PRINT(RED("WARNING: this formatting method is not accurate\n"));
            int32_t *p = data;
            PRINT(BLUE("%016" PRIX64) " %d\n", val.addr + i * sizeof(*p), p[i]);
        }
        if (fmt == 'b')
        {
            uint8_t *p = data;
            PRINT(BLUE("%016" PRIX64) " 0x%02hhx\n", val.addr + i * sizeof(*p), p[i]);
        }
        if (fmt == 'c')
        {
            char *p = data;
            PRINT(BLUE("%016" PRIX64) " %c\n", val.addr + i * sizeof(*p), p[i]);
        }
        print_warning = false;
    }
}

void print_op(tracee *tracee, char *cmd)
{
    LOG_DEBUG("operation PRINT");
    char buf[BUFSIZ] = {0};
    char fullfmt[BUFSIZ] = {0};
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
    PRINT(fullfmt, buf, val);
}

void breakpoint_op(tracee *tracee, char *cmd)
{
    LOG_DEBUG("operation BREAKPOINT");
    if (strlen(cmd) == 1)
    {
        breakpoint_list(tracee);
    }
    if (cmd[1] == ' ')
    {
        Value addr = resolve_value(tracee, &cmd[2]);
        if (!addr.addr)
        {
            PRINT(RED("address does not exists\n"));
        }
        else
        {
            breakpoint_set(tracee, addr.addr);
        }
    }
    if (cmd[1] == 'd')
    {
        Value addr = resolve_value(tracee, &cmd[3]);
        if (!addr.addr)
        {
            PRINT(RED("address does not exists\n"));
        }
        else
        {
            breakpoint_unset(tracee, addr.addr);
        }
    }
}

void help_op(tracee *tracee, char *cmd)
{
    char buffer[BUFSIZ] = {0};
    FILE *helpfp = fopen("./docs/help.txt", "r");
    fread(buffer, sizeof(*buffer), BUFSIZ, helpfp);
    PRINT("%s\n", buffer);
}

void quit_op(tracee *tracee, char *cmd)
{
    PRINT(YELLOW("Goodby from gdb!\n"));
}

void info_op(tracee *tracee, char *cmd)
{
    LOG_DEBUG("INFO OPERATION");
    char type;
    if (sscanf(cmd, "i %c", &type) != 1)
    {
        LOG_ERROR("invalid input");
        return;
    }
    // symbols
    if (type == 's')
    {
        GElf_Addr base_addr = symtab_get_dyn_sym_addr(tracee->pid, tracee->symtab.symbols);
        for (size_t i = 0; i < tracee->symtab.size; i++)
        {
            GElf_Addr sym_value = tracee->symtab.symbols[i].st_value;
            PRINT(BLUE("0x%016lx") " %s\n", base_addr + sym_value, tracee->symtab.sym_names[i]);
        }
    }
    else if (type == 'r')
    {
        struct user_regs_struct regs = get_tracee_registers(tracee);
        reg_t *reg = &regs;
        for (size_t i = 0; i < COUNT_REGS(regs); i++, reg++)
        {
            PRINT(BLUE("%8s") " = 0x%lx\n", defined_regs[i], *reg);
        }
    }
    else
    {
        PRINT(RED("undefined option after i(nfo)\n"));
    }
    PRINT("\n");
}