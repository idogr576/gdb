#pragma once
#include <sys/types.h>

#include "tracee.h"

#define OPCODE_MAX_SIZE 15
#define OPCODE_MAX_REPR 256

void get_current_opcode(tracee *tracee, char *opcode);
