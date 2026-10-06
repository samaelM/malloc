#include <ft_malloc.h>

void ft_free(void *ptr) {
    if (!ptr) return;
    
    void        *memoryBlock    =  GET_BLOCK(ptr);
    pool_t      **currentPool   =  findPool(GET_SIZE(memoryBlock));
    pool_t      *currentPage    =  findPage(*currentPool, memoryBlock);
    
    if (!currentPage) return;
    
    // get the memory block position (with header)
    *(size_t *)memoryBlock      ^= 1;
    DEBUG("memory freed");

    if (isEmptyPage(currentPage)) {
        // ft_show_alloc_mem();
        deletePage(currentPool, currentPage);
    }

    return;
}
