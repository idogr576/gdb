#pragma once
#include <gelf.h>
#include <sys/types.h>

#include "tracee.h"

void breakpoint_init(tracee *tracee);

void breakpoint_list(tracee *tracee);

void breakpoint_set(tracee *tracee, GElf_Addr addr);

void breakpoint_unset(tracee *tracee, GElf_Addr addr);

void breakpoint_step(tracee *tracee);

uint8_t breakpoint_memset(tracee *tracee, GElf_Addr addr, uint8_t value);
