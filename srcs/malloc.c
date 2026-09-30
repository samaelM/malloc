#include <ft_malloc.h>

pool_t   *tinyPool;      // from 0 to 1024
pool_t   *smallPool;     // from 1024 to 16k
pool_t   *largePool;     // from 16k to infinite (one block per page)

void *ft_malloc(size_t size) {
    if (size == 0)
        return (NULL);

    pool_t          **currentPool;
    void            *pointer;
    const size_t    realSize = ALIGN(size + sizeof(size_t));
    size_t          poolSize = TINY_POOL_SIZE;

    // selecting the best pool for size
    if (realSize <= TINY_POOL_ACCEPT) {
        currentPool = &tinyPool;
    } else if (realSize <= SMALL_POOL_ACCEPT) {
        currentPool = &smallPool;
        poolSize = SMALL_POOL_SIZE;
    } else {
        currentPool = &largePool;
        poolSize = realSize + sizeof(pool_t *) + sizeof(size_t);
    }

    // insert the header in the pools
    pointer = insertBlockPool(*currentPool, realSize);
    if (!pointer) {
        poolAddBack(currentPool, poolSize);
        pointer = insertBlockPool(poolLast(*currentPool), realSize);
    }

    return (pointer);
}
