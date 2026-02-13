#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <logger.h>

#include "breakpoint.h"
#include "registers.h"
#include "hashmap.h"

#define BP_OPCODE 0xcc
#define BP_OPCODE_SIZE 1

void breakpoint_init(tracee *tracee)
{
    LOG_DEBUG("initialize breakpoint hashmap");
    hminit(tracee->breakpoints);
}

void breakpoint_list(tracee *tracee)
{
    LOG_DEBUG("listing breakpoints");
    LOG_DEBUG("found %d breakpoints\n", HASH_COUNT(tracee->breakpoints));
    // for (int i = 0; i < HASH_COUNT(hash); i++)
    // {
    //     printf("[%d] 0x%lx\n", i, hash[i].key);
    // }

    hash_t current, tmp;
    int i = 0;
    HASH_ITER(hh, tracee->breakpoints, current, tmp)
    {
        printf("[%d]\t*0x%lx\n", i++, current->key);
    }
    puts("");
}

void breakpoint_set(tracee *tracee, GElf_Addr addr)
{
    if (hmfind(tracee->breakpoints, addr))
    {
        printf("breakpoint at 0x%lx already set\n", addr);
        return;
    }
    // if (ptrace(PTRACE_POKEDATA, pid, addr, BP_OPCODE) == -1)
    // {
    //     LOG_DEBUG("got error in breakpoint set!");
    //     perror("");
    //     return;
    // }
    // hash_t e = malloc(sizeof(*e));
    // if (!e)
    // {
    //     LOG_ERROR("cannot malloc new hash entry");
    //     return;
    // }
    // e->key = addr;
    // e->value = orig_data;
    // HASH_ADD(hh, *hash, key, sizeof(e->key), e);
    char orig = breakpoint_memset(tracee, addr, BP_OPCODE);
    hmput(&tracee->breakpoints, addr, orig);
    printf("added new breakpoint at *0x%lx\n", addr);
    LOG_DEBUG("there are now %d breakpoints\n", HASH_COUNT(tracee->breakpoints));
}

void breakpoint_unset(tracee *tracee, GElf_Addr addr)
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
    hash_t found = hmfind(tracee->breakpoints, addr);
    if (!found)
    {
        printf("did not find breakpoint at 0x%lx\n", addr);
        return;
    }
    breakpoint_memset(tracee, addr, found->value);
    hmdel(&tracee->breakpoints, addr);
    printf("deleted breakpoint at 0x%lx\n", addr);
}

void breakpoint_step(tracee *tracee)
{
    /*
    1. check if current rip is on a breakpoint. if not, return
    2. write original byte to tracee's mem
    3. move rip 1 bytes back
    4. move a single step
    5. write the opcode back
    */
    // step 1
    struct user_regs_struct regs = get_tracee_registers(tracee);
    GElf_Addr bprip = (GElf_Addr)regs.rip - BP_OPCODE_SIZE;
    hash_t found = hmfind(tracee->breakpoints, bprip);
    if (!found)
    {
        return;
    }
    // step 2
    breakpoint_memset(tracee, bprip, found->value);

    // step 3
    set_register_value(tracee, "rip", bprip);

    // step 4
    ptrace(PTRACE_SINGLESTEP, tracee->pid, 0, 0);
    waitpid(tracee->pid, 0, 0);

    // step 5
    breakpoint_memset(tracee, bprip, BP_OPCODE);
}

char breakpoint_memset(tracee *tracee, GElf_Addr addr, char value)
{
    char orig = 0;
    union
    {
        void *word;
        char byte;
    } data;
    data.word = (void *)ptrace(PTRACE_PEEKDATA, tracee->pid, addr, 0);
    orig = data.byte;
    data.byte = value;
    ptrace(PTRACE_POKEDATA, tracee->pid, addr, data.word);
    return orig;
}
