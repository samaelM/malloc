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

static void mergeBlocks(char *current) {
    size_t      size = GET_SIZE(current);
    char        *next = current + size;
    bool        is_used_next = GET_FLAG(next);

    while (!is_used_next) {
        size = GET_SIZE(next);
        if (size == 0) { // if it's the last block?// remplacer par next == pool.data+pool.size
            // printf("last block?%p\n", next);
            return;
        }
        *(size_t *)current += size;
        DEBUG("merged block");
        next = next + size;
        is_used_next = GET_FLAG(next);
    }
}

void *insertBlockPool(pool_t *pool, const size_t header) {
    if (!pool) return (NULL);
    char        *current = pool->data;
    int         safeIndex = 0;

    while (current < pool->data + pool->size) {
        safeIndex++;
        if (safeIndex > 5) return (DEBUG("merge safeIndex"),NULL);
        bool    is_used = GET_FLAG(current);
        if (!is_used) {
            mergeBlocks(current);
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
