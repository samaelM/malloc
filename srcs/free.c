#include <ft_malloc.h>

void ft_free(void *ptr) {
    if (!ptr) return;
    
    void        *memoryBlock    =  GET_BLOCK(ptr);
    pool_t      **currentPool   =  findPool(GET_SIZE(memoryBlock));
    pool_t      *currentPage    =  findPage(*currentPool, memoryBlock);
    
    if (!currentPage) return;

    *(size_t *)memoryBlock      ^= 1;

    if (isEmptyPage(currentPage)) {
        deletePage(currentPool, currentPage);
    }

    return;
}
