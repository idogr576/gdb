#pragma once

typedef enum command {
    UNDEFINED,
    RUN,
    CONTINUE,
    NEXT,
    EXAMINE,
    PRINT,
    BREAKPOINT
} command;


command read_command();