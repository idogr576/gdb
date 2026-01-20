#pragma once
/*
TODO: implement symtab loading from elf using libelf.

found this stack overflow thread that might help!
https://stackoverflow.com/questions/23809102/print-the-symbol-table-of-an-elf-file

also, this is the source code of the nm (list elf symbols) command
https://sourceware.org/git/?p=elfutils.git;a=blob;f=src/nm.c;h=40b192f67b924eaba5d3d2dfa71bc8b09ba40654;hb=HEAD#l1572
*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libelf.h>
#include <gelf.h>

#define SYMTAB_SIZE 1000
#define SYM_MAX_LEN 100

typedef struct symbolTable
{
    size_t size;
    char sym_names[SYMTAB_SIZE][SYM_MAX_LEN];
    GElf_Sym symbols[SYMTAB_SIZE];
} symtab;

extern symtab g_symtab;

void symtab_init(symtab *symtab);

void symtab_add_sym(symtab *symtab, char *sym_name, GElf_Sym *sym);

void symtab_elf_load(const char *elf_path, symtab *symtab);

GElf_Sym *symtab_find_sym(symtab *symtab, char *symname);

// only after tracee runs execv
GElf_Addr symtab_get_dyn_sym_addr(pid_t pid, GElf_Sym *sym);
