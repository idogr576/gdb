#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libelf.h>
#include <gelf.h>
#include <string.h>
#include <stdbool.h>

#include "logger.h"
#include "print.h"
#include "elf/symbols.h"

#define PATH_MAX_LEN 100

void symtab_init(symtab *symtab)
{
    LOG_DEBUG("initialize symtab");
    symtab->size = 0;
}

void symtab_add_sym(symtab *symtab, char *sym_name, GElf_Sym *sym)
{
    size_t size = symtab->size;
    symtab->symbols[size] = *sym;
    char *new_sym_name = symtab->sym_names[size];
    strcpy(new_sym_name, sym_name);
    symtab->size++;
}

/*
this function assumes that the elf_path is valid and given from main
*/
void symtab_elf_load(const char *elf_path, symtab *symtab)
{
    Elf *elf = NULL;
    Elf_Scn *scn = NULL;
    GElf_Shdr shdr;
    Elf_Data *data;
    int fd = -1;
    int ii, count;
    bool found_symtab = false;

    if (elf_version(EV_CURRENT) == EV_NONE)
    {
        PRINT(RED("did not find an elf version for the binary\n"));
        goto ret;
    }

    fd = open(elf_path, O_RDONLY);
    elf = elf_begin(fd, ELF_C_READ, NULL);

    while (!found_symtab && (scn = elf_nextscn(elf, scn)) != NULL)
    {
        gelf_getshdr(scn, &shdr);
        if (shdr.sh_type == SHT_SYMTAB)
        {
            /* found a symbol table, go print it. */
            LOG_INFO("found a symtab!");
            found_symtab = true;
        }
    }

    if (!found_symtab)
    {
        LOG_INFO("binary does not have any symbols to load");
        goto ret;
    }

    data = elf_getdata(scn, NULL);
    count = shdr.sh_size / shdr.sh_entsize;

    /* load the symbols */
    symtab_init(symtab);
    for (ii = 0; ii < count; ++ii)
    {
        GElf_Sym sym;
        gelf_getsym(data, ii, &sym);
        char *sym_name = elf_strptr(elf, shdr.sh_link, sym.st_name);
        symtab_add_sym(symtab, sym_name, &sym);
    }

ret:
    if (elf)
    {
        elf_end(elf);
    }
    if (fd != -1)
    {
        close(fd);
    }
}

GElf_Sym *symtab_find_sym(symtab *symtab, char *symname)
{
    for (size_t i = 0; i < symtab->size; i++)
    {
        if (!strcmp(symtab->sym_names[i], symname))
        {
            LOG_DEBUG("found symbol %s at index %d", symname, i);
            return &(symtab->symbols[i]);
        }
    }
    return NULL;
}

GElf_Addr symtab_get_dyn_sym_addr(pid_t pid, GElf_Sym *sym)
{
    char maps[PATH_MAX_LEN] = {0};
    char buff[BUFSIZ] = {0};
    char *sep;
    FILE *fp;
    GElf_Addr base_addr, sym_addr = (GElf_Addr){0};

    snprintf(maps, sizeof(maps), "/proc/%d/maps", pid);

    fp = fopen(maps, "r");
    if (!fp)
    {
        LOG_ERROR("cannot open %s for reading", maps);
        goto ret;
    }
    fgets(buff, sizeof(buff), fp);

    sep = strchr(buff, '-');

    if (!sep)
    {
        LOG_ERROR("cannot find '-' in maps");
        goto ret;
    }

    *sep = '\0';

    LOG_DEBUG("first line is: %s", buff);

    base_addr = (GElf_Addr)strtoull(buff, NULL, 16);
    sym_addr = base_addr + sym->st_value;

ret:
    fclose(fp);
    return sym_addr;
}