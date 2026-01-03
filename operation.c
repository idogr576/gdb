#include <logger.h>
#include "operation.h"

void run_op(char *cmd)
{
    LOG_DEBUG("operarion RUN");
}

void continue_op(char *cmd)
{
    LOG_DEBUG("operation CONTINUE");
}

void next_op(char *cmd)
{
    LOG_DEBUG("operation NEXT");
}

void examine_op(char *cmd)
{
    LOG_DEBUG("operation EXAMINE");
}

void print_op(char *cmd)
{
    LOG_DEBUG("operation PRINT");
}

void breakpoint_op(char *cmd)
{
    LOG_DEBUG("operation BREAKPOINT");
}
