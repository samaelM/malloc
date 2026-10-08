#include <ft_malloc.h>

gPool_t g_pools = {0};

void *malloc(size_t size) {
    if (size == 0) return (NULL);

    pool_t          **currentPool;
    void            *pointer;
    const size_t    realSize = ALIGN(size + sizeof(size_t));
    size_t          poolSize;

    // selecting the best pool for size
    currentPool = findPool(realSize);
    if (currentPool == &g_pools.tinyPool)       poolSize = TINY_POOL_SIZE;
    else if (currentPool == &g_pools.smallPool) poolSize = SMALL_POOL_SIZE;
    else if (currentPool == &g_pools.largePool) poolSize = realSize + sizeof(pool_t *) + sizeof(size_t);

    // insert the header in the pools
    pointer = insertBlockPool(*currentPool, realSize);
    if (!pointer) {
        poolAddBack(currentPool, poolSize);
        pointer = insertBlockPool(poolLast(*currentPool), realSize);
    }

    return (pointer);
}
