#include <ft_malloc.h>

extern pool_t *tinyPool;

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
                if (size <= 0 || size >100000) {
                    printf("bizarre->");
                }
                printf("\t%p - %p : %lu bytes (used)\n", currentBlock, currentBlock + size, size);
                currentBlock += size;
            } else {
                printf("\t%p - %p : %lu bytes (unused)\n", currentBlock, currentBlock + size, size);
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

void ft_show_alloc_mem() {
    printPool(tinyPool, "TINY");
    printPool(smallPool, "SMALL");
    printPool(largePool, "LARGE");
}

// print the memory as hex dump
// for (size_t i = 0; i < tinyPool.size; i++) {
//     if (i % 16 == 0)
//         printf("\n%p: ", tinyPool.data + i);
//     printf("%02x ", (unsigned char)tinyPool.data[i]);
// }
