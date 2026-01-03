#include <sys/ptrace.h>
#include <logger.h>
#include "operation.h"

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
