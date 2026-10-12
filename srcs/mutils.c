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


// size_t  ft_ltoa_hexa_size(unsigned long long nb)
// {
//     size_t  lenght;
//     lenght = 0;
//     while (nb > 0) {
//         nb /= 16;
//         lenght++;
//     }
//     return (lenght);
// }

// size_t ft_printPointer(char *dest, void *n) {
//     size_t              index;
//     size_t              lenght;
//     unsigned long long  num;

//     if (n == 0) return (ft_strlcpy(dest, "0", 2));

//     num = (unsigned long long)n;
//     lenght = ft_ltoa_hexa_size(num);
//     index = lenght;
//     dest[index--] = '\0';
//     while (num > 0) {
//         dest[index] = "0123456789abcdef"[(num % 16)];
//         num /= 16;
//         index--;
//     }

//     return (lenght);
// }
