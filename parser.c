#include <stdlib.h>
#include <string.h>
#include <logger.h>

#include "symbols.h"
#include "parser.h"

AddrType identify_addr_type(char *addr_repr, symtab *symtab)
{
    if (addr_repr[0] == '*')
    {
        return DIRECT;
    }
    return symtab_find_sym(symtab, addr_repr) == NULL ? INVALID : SYMBOL;
}

GElf_Addr parse_direct_address(char *addr_repr)
{
    // char *addr_bases[] = {"*0x", "*", NULL};

    // int idx_base = 0;
    // char *selected_base;
    // do
    // {
    //     selected_base = addr_bases[idx_base++];
    //     if (!strncmp(addr_repr, selected_base, strlen(selected_base)))
    //     {
    //         break;
    //     }
    // } while (selected_base != NULL);

    if (!strncmp(addr_repr, "*0x", 3))
    {
        return (GElf_Addr)strtoull(addr_repr + 3, NULL, 16);
    }
    return (GElf_Addr)-1;
}

GElf_Addr resolve_address(int pid, symtab *symtab, char *addr_repr)
{
    AddrType addr_type = identify_addr_type(addr_repr, symtab);
    if (addr_type == DIRECT)
    {
        LOG_DEBUG("found direct address: %s", addr_repr);
        return parse_direct_address(addr_repr);
    }
    if (addr_type == SYMBOL)
    {
        GElf_Sym *sym = symtab_find_sym(symtab, addr_repr);
        if (!sym)
        {
            goto error;
        }

        GElf_Addr addr = symtab_get_dyn_sym_addr(pid, sym);
        LOG_DEBUG("%s == 0x%lx\n", addr_repr, addr);
        return addr;
    }
error:
    return (GElf_Addr)-1;
}