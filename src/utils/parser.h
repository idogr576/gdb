#pragma once
#include <gelf.h>
#include <sys/types.h>

#include "elf/symbols.h"
#include "registers.h"
#include "tracee.h"

typedef enum AddrType
{
    DIRECT,
    SYMBOL,
    INVALID,
} AddrType;

typedef enum ValueType
{
    TYPE_ADDRESS,
    TYPE_SYMBOL,
    TYPE_REGISTER,
    TYPE_INVALID,
} ValueType;

typedef union Value
{
    GElf_Addr addr;
    reg_t reg;
} Value;

ValueType identify_addr_type(char *addr_repr, symtab *symtab);

GElf_Addr parse_direct_address(char *addr_repr);

// GElf_Addr parse_symbol_address(char* addr_repr, pid_t pid, symtab* symtab);

GElf_Addr resolve_address(ValueType type, pid_t pid, symtab *symtab, char *addr_repr);

Value resolve_value(tracee *tracee, char *addr_repr);
