#include <ft_malloc.h>

void ft_free(void *ptr) {
    if (!ptr) return;
    DEBUG("memory freed");
    // get the memory block position (with header)
    void        *memoryBlock = GET_BLOCK(ptr);

    // fliping the is_used flag
    *(size_t *)memoryBlock ^= 1;


    pool_t **currentPool = findPool(GET_SIZE(memoryBlock));
    pool_t *currentPage = findPage(*currentPool, memoryBlock);
    if (isEmptyPage(currentPage)) {
        deletePage(currentPool, currentPage);
    }

}
