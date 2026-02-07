#pragma once
#include <sys/user.h>
#include <bits/types/error_t.h>
#include "tracee.h"

typedef unsigned long long reg_t;

extern const char *defined_regs[];

struct user_regs_struct get_tracee_registers(tracee *tracee);

reg_t get_register_value(tracee *tracee, char *reg_name);

error_t set_register_value(tracee *tracee, char *reg_name, reg_t value);
