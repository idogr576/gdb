#pragma once
#include "gelf.h"
#include "uthash.h"

typedef struct
{
    GElf_Addr key;
    char value;
    UT_hash_handle hh; // makes it hashable
} HashEntry;

typedef HashEntry *hm_t;

void hminit(hm_t hash);

void hmput(hm_t *hash, GElf_Addr key, char value);

void hmdel(hm_t *hash, GElf_Addr key);

hm_t hmfind(hm_t hash, GElf_Addr key);
