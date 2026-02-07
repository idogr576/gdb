#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <logger.h>

#include "breakpoint.h"
#include "hashmap.h"

#define BP_OPCODE 0xcc

void breakpoint_init(hash_t hash)
{
    LOG_DEBUG("initialize breakpoint hashmap");
    hminit(hash);
}

void breakpoint_list(hash_t hash)
{
    LOG_DEBUG("listing breakpoints");
    LOG_DEBUG("found %d breakpoints\n", HASH_COUNT(hash));
    // for (int i = 0; i < HASH_COUNT(hash); i++)
    // {
    //     printf("[%d] 0x%lx\n", i, hash[i].key);
    // }

    HashEntry *current, *tmp;
    int i = 0;
    HASH_ITER(hh, hash, current, tmp)
    {
        printf("[%d] *0x%lx\n", i++, current->key);
    }
    puts("");
}

void breakpoint_set(hash_t *hash, pid_t pid, GElf_Addr addr)
{
    if (hmfind(*hash, addr))
    {
        printf("breakpoint at 0x%lx already set\n", addr);
        return;
    }
    char orig_data = (char)ptrace(PTRACE_PEEKDATA, pid, addr, 0);
    // if (ptrace(PTRACE_POKEDATA, pid, addr, BP_OPCODE) == -1)
    // {
    //     LOG_DEBUG("got error in breakpoint set!");
    //     perror("");
    //     return;
    // }
    hmput(hash, addr, orig_data);
    // hash_t e = malloc(sizeof(*e));
    // if (!e)
    // {
    //     LOG_ERROR("cannot malloc new hash entry");
    //     return;
    // }
    // e->key = addr;
    // e->value = orig_data;
    // HASH_ADD(hh, *hash, key, sizeof(e->key), e);
    ptrace(PTRACE_POKEDATA, pid, addr, 0xcc);
    printf("added new breakpoint at *0x%lx\n", addr);
    LOG_DEBUG("%d\n", HASH_COUNT(*hash));
}

void breakpoint_unset(hash_t *hash, pid_t pid, GElf_Addr addr)
{
    // char orig_data = hmget(hash, addr);
    // if (orig_data == NULL)
    // {
    //     printf("breakpoint at 0x%lx does not exists\n", addr);
    //     return;
    // }
    // if (ptrace(PTRACE_POKEDATA, pid, addr, orig_data) == -1)
    // {
    //     LOG_DEBUG("got error in breakpoint unset!");
    //     perror("");
    //     return;
    // }
    // hmdel(hash, addr);
    hmdel(hash, addr);
    printf("deleted breakpoint at 0x%lx\n", addr);
}
