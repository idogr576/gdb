#include <stdio.h>
#include <sys/ptrace.h>
#include <logger.h>
#include "operation.h"

#define BUF_SIZE 1000

void run_op(int pid, char *cmd)
{
    LOG_DEBUG("operarion RUN");
}

void continue_op(int pid, char *cmd)
{
    LOG_DEBUG("operation CONTINUE");
    ptrace(PTRACE_CONT, pid, 0, 0);
}

void next_op(int pid, char *cmd)
{
    LOG_DEBUG("operation NEXT");
}

void examine_op(int pid, char *cmd)
{
    LOG_DEBUG("operation EXAMINE");
}

void print_op(int pid, char *cmd)
{
    LOG_DEBUG("operation PRINT");
}

void breakpoint_op(int pid, char *cmd)
{
    LOG_DEBUG("operation BREAKPOINT");
    LOG_DEBUG("pid is %d, cmd is \"%s\"", pid, cmd);
}

void help_op(int pid, char *cmd)
{
    char buffer[BUF_SIZE] = {0};
    FILE *helpfp = fopen("./docs/help.txt", "r");
    fread(buffer, sizeof(*buffer), BUF_SIZE, helpfp);
    puts(buffer);
}
