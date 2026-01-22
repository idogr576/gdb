#pragma once
#include <gelf.h>
#include "symbols.h"

typedef enum AddrType {
    DIRECT,
    SYMBOL,
    INVALID,
} AddrType;

AddrType identify_addr_type(char *addr_repr, symtab* symtab);

GElf_Addr parse_direct_address(char* addr_repr);

// GElf_Addr parse_symbol_address(char* addr_repr, int pid, symtab* symtab);

GElf_Addr resolve_address(int pid, symtab* symtab, char* addr_repr);
