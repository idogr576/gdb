#include <logger.h>

#include "hashmap.h"

void hminit(hm_t hash)
{
    hash = NULL;
}

void hmput(hm_t *hash, GElf_Addr key, char value)
{
    hm_t e = malloc(sizeof(*e));
    if (!e)
    {
        LOG_ERROR("cannot malloc new hash entry");
        return;
    }
    e->key = key;
    e->value = value;
    HASH_ADD(hh, *hash, key, sizeof(e->key), e);
}

void hmdel(hm_t *hash, GElf_Addr key)
{
    hm_t delkey = hmfind(*hash, key);
    if (!delkey)
    {
        return;
    }
    HASH_DEL(*hash, delkey);
    free(delkey);
}

hm_t hmfind(hm_t hash, GElf_Addr key)
{
    hm_t found = NULL;
    HASH_FIND(hh, hash, &key, sizeof(key), found);
    return found;
}
