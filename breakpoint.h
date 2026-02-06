#pragma once
#include <gelf.h>
#include <sys/types.h>

#include "hashmap.h"

void breakpoint_init(hash_t hash);

void breakpoint_list(hash_t hash);

void breakpoint_set(hash_t *hash, pid_t pid, GElf_Addr addr);

void breakpoint_unset(hash_t *hash, pid_t pid, GElf_Addr addr);
