#pragma once
#include "gelf.h"
#include "uthash.h"

typedef struct
{
    GElf_Addr key;
    char value;
    UT_hash_handle hh; // makes it hashable
} HashEntry;

typedef HashEntry *hash_t;

void hminit(hash_t hash);

void hmput(hash_t *hash, GElf_Addr key, char value);

void hmdel(hash_t *hash, GElf_Addr key);

hash_t hmfind(hash_t hash, GElf_Addr key);
