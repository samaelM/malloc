#include <ft_malloc.h>

static pool_t *initPool(const size_t size) {
    pool_t      *memory = mmap( NULL,
                                size,
                                PROT_READ | PROT_WRITE,
                                MAP_ANON | MAP_SHARED,
                                -1,
                                0  * PAGESIZE
    );

    if (memory == MAP_FAILED) {
        DEBUG("MAP_FAILED");
        return (NULL);
    }

    memory->size = size - sizeof(size_t) - sizeof(pool_t *);
    memory->next = NULL;
    *(size_t *)memory->data = size - sizeof(size_t) - sizeof(pool_t *);

    return (memory);
}

pool_t **findPool(size_t size) {
    if (size <= TINY_POOL_ACCEPT) {
        return (&tinyPool);
    } else if (size <= SMALL_POOL_ACCEPT) {
        return (&smallPool);
    } else {
        return (&largePool);
    }
}

bool isEmptyPage(pool_t *pool) {
    if (!pool) return (FALSE);
    char        *current = pool->data;
    size_t      size = GET_SIZE(current);
    bool        is_used = GET_FLAG(current);

    while (!is_used) {
        DEBUG("unused block");
        size = GET_SIZE(current);
        current = current + size;
        if (current >= pool->data+pool->size) {
            return (TRUE);
        }
        is_used = GET_FLAG(current);
    }

    // printf("%p (%ld bytes) is used\n", next, size);
    return (FALSE);
}

pool_t *findPage(pool_t *pool, void *addr) {
    while (addr < (void *)pool || addr > (void *)(pool->data+pool->size)) {
        pool = pool->next;
        if (pool->next == NULL) return (NULL);
    }

    return (pool);
}

void deletePage(pool_t **pool, pool_t *page) {
    if (!pool) return;

    if (*pool == page) {
        pool_t *bad = *pool;
        *pool = bad->next;
        DEBUG("deleting page");
        munmap(bad, bad->size + 16);

        return;
    }

    pool_t *current = *pool;
    while (current->next){
        if (current->next == page) {
            pool_t *bad = current->next;
            current->next = bad->next;
            DEBUG("deleting page");
            munmap(bad, bad->size + 16);

            return;
        }
    }
    printf(FLASH RED "SOMETHING WENT BAD\n" WHITE);
}

void mergeBlocks(char *current, pool_t *pool) {
    size_t      size = GET_SIZE(current);
    char        *next = current + size;
    if (!next || next >= pool->data+pool->size) return;
    bool        is_used_next = GET_FLAG(next);

    while (!is_used_next) {
        size = GET_SIZE(next);
        *(size_t *)current += size;
        DEBUG("merged block");
        next = next + size;
        if (next >= pool->data+pool->size) {
            return;
        }
        is_used_next = GET_FLAG(next);
    }
}

void *insertBlockPool(pool_t *pool, const size_t header) {
    if (!pool) return (NULL);
    char        *current = pool->data;
    int         safeIndex = 0;

    while (current < pool->data + pool->size) {
        safeIndex++;
        if (safeIndex > 5) return (NULL);
        bool    is_used = GET_FLAG(current);
        if (!is_used) {
            mergeBlocks(current, pool);
        }
        size_t  size = GET_SIZE(current);

        // If the block is used or the size available is too small.
        if (is_used || size < header) {
            DEBUG("block skip");
            current += size;
            if (size == 0) { // if it's the last block?
                DEBUG("ohoh problemo");
                return NULL;
            }
        } else {
            size_t  spaceLeft = size;
            size = header | 1;
            if (spaceLeft > header) {
                *(size_t *)(current + header) = spaceLeft - header;
            }
            *(size_t *)current = size;
            DEBUG("found space");
            return (current + sizeof(size_t));
        }
    }
    DEBUG("not enough space in pool");
    return (NULL);
}

void *insertBlockPoolHinted(pool_t *pool, const size_t header, void *hint) {
    if (!pool)      return (NULL);
    if (!hint)      return (NULL);
    if (!header)    return (NULL);

    void    *nextBlock  = NEXT_BLOCK(hint);
    size_t  next_size   = GET_SIZE(nextBlock);
    size_t  spaceLeft   = next_size + GET_SIZE(hint);
    if (spaceLeft > header) {
        *(size_t *)(hint + header) = spaceLeft - header;
    }
    *(size_t *)hint = header | 1;

    return hint;
}

pool_t *poolLast(pool_t *pool) {
    if (!pool) return (NULL);

    while (pool->next) {
        pool = pool->next;
    }
    return (pool);
}

int poolAddBack(pool_t **pool, const size_t size) {
    pool_t      *new = initPool(size);
    if (new == NULL) return (0);
    DEBUG("new pool");

    if (!*pool){
        *pool = new;
    } else {
        poolLast(*pool)->next = new;
    }
    return (1);
}
