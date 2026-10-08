#include <ft_malloc.h>

void printPool(const pool_t *pool, const char *name){
    const pool_t    *currentPool = pool;
    printf(GREEN "%s : %p - %p\n" WHITE, name, currentPool, (currentPool?currentPool->data+currentPool->size: NULL));
    short       pageIndex = 0;

    while (currentPool) {
        const char  *currentBlock = currentPool->data;
        printf(YELLOW "PAGE %d: %ld bytes\n" WHITE, pageIndex, currentPool->size);

        while (currentBlock < currentPool->data + currentPool->size) {
            const size_t    size = GET_SIZE(currentBlock);
            const bool      is_used = GET_FLAG(currentBlock);
    
            if (is_used) {
                printf("\t%p - %p : %lu bytes\t(used)\n", currentBlock, currentBlock + size, size);
                currentBlock += size;
            } else {
                printf("\t%p - %p : %lu bytes\t(unused)\n", currentBlock, currentBlock + size, size);
                currentBlock += size;
                if (size == 0) {
                    break;
                }
            }
        }
        currentPool = currentPool->next;
        pageIndex++;
    }
    printf("\n");
}

void show_alloc_mem() {
    printPool(g_pools.tinyPool, "TINY");
    printPool(g_pools.smallPool, "SMALL");
    printPool(g_pools.largePool, "LARGE");
}

