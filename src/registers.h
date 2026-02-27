#pragma once
#include <sys/user.h>
#include <bits/types/error_t.h>
#include <gelf.h>
#include "tracee.h"

#define INVALID_REGISTER_VALUE -1

#define COUNT_REGS(regs) (sizeof(regs) / sizeof(reg_t))

typedef unsigned long long reg_t;

extern const char *defined_regs[];

struct user_regs_struct get_tracee_registers(tracee *tracee);

reg_t get_register_value(tracee *tracee, char *reg_name);

GElf_Addr get_program_counter(tracee *tracee);

error_t set_register_value(tracee *tracee, char *reg_name, reg_t value);
