#include <stdlib.h>
#include <string.h>
#include "logger.h"

#include "elf/symbols.h"
#include "utils/parser.h"

ValueType identify_addr_type(char *addr_repr, symtab *symtab)
{
    if (addr_repr[0] == '*')
    {
        return TYPE_ADDRESS;
    }
    if (addr_repr[0] == '$')
    {
        return TYPE_REGISTER;
    }
    return symtab_find_sym(symtab, addr_repr) == NULL ? TYPE_INVALID : TYPE_SYMBOL;
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

GElf_Addr resolve_address(ValueType type, pid_t pid, symtab *symtab, char *addr_repr)
{
    if (type == TYPE_ADDRESS)
    {
        LOG_DEBUG("found direct address: %s", addr_repr);
        return parse_direct_address(addr_repr);
    }
    if (type == TYPE_SYMBOL)
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

Value resolve_value(tracee *tracee, char *addr_repr)
{
    Value ret = {0};
    ValueType type = identify_addr_type(addr_repr, &tracee->symtab);
    // check if register or address
    switch (type)
    {
    case TYPE_REGISTER:
        reg_t reg = get_register_value(tracee, addr_repr + 1);
        ret.reg = reg;
        break;

    case TYPE_ADDRESS:
    case TYPE_SYMBOL:
        GElf_Addr addr = resolve_address(type, tracee->pid, &tracee->symtab, addr_repr);
        if (addr == (GElf_Addr)-1)
        {
            LOG_ERROR("cannot resolve address %s", addr_repr);
        }
        else
        {
            ret.addr = addr;
        }
        break;
    default:
        LOG_ERROR("address does not match any kind");
    }
    return ret;
}