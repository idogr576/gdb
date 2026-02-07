#include <logger.h>

#include "hashmap.h"

void hminit(hash_t hash)
{
    hash = NULL;
}

void hmput(hash_t *hash, GElf_Addr key, char value)
{
    hash_t e = malloc(sizeof(*e));
    if (!e)
    {
        LOG_ERROR("cannot malloc new hash entry");
        return;
    }
    e->key = key;
    e->value = value;
    HASH_ADD(hh, *hash, key, sizeof(e->key), e);
}

void hmdel(hash_t *hash, GElf_Addr key)
{
    hash_t delkey = hmfind(*hash, key);
    if (!delkey)
    {
        return;
    }
    HASH_DEL(*hash, delkey);
    free(delkey);
}

hash_t hmfind(hash_t hash, GElf_Addr key)
{
    hash_t found = NULL;
    HASH_FIND(hh, hash, &key, sizeof(key), found);
    return found;
}
