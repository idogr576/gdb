#pragma once
#include <stddef.h>
#include "tracee.h"

size_t read_tracee_mem(tracee *tracee, long* addr, int* data, size_t n);
