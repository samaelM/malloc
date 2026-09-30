#include <ft_malloc.h>

void ft_free(void *ptr) {
    if (!ptr) return;
    DEBUG("memory freed");
    // get the memory block position (with header)
    void        *memoryBlock = GET_BLOCK(ptr);

    // fliping the is_used flag
    *(size_t *)memoryBlock -= 1;

    // if pool page is empty, munmap it, and don't break the chain

    // get the size of the block to get pool
    // size_t size = (*(size_t *)memoryBlock >> 3) << 3;
    // find the owner of the pointer

}
