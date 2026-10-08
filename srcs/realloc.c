#include <ft_malloc.h>

void *changSize(void *ptr, size_t oldSize, size_t newSize, size_t size) {
    if (newSize < oldSize) {
        return (ptr);
    }
    void *new = ft_malloc(size);
    memcpy(new, ptr, size);
    ft_free(ptr);
    return (new);
}

void    *ft_realloc(void *ptr, size_t size) {
    DEBUG("--- entering realloc ---");
    /*  Pointer does not exist (POSIX)  */
    if (!ptr) {
        void *new = ft_malloc(size);

        return (new);
    }

    void        *memoryBlock    = GET_BLOCK(ptr);
    pool_t      **currentPool   = findPool(GET_SIZE(memoryBlock));
    size_t      oldSize         = GET_SIZE(memoryBlock) - sizeof(size_t);
    size_t      newSize         = ALIGN(size + sizeof(size_t));

    /*  Pointer is in large page    */
    if (currentPool == &largePool) {
        return (changSize(ptr, oldSize, newSize, size));
    }
    pool_t      *currentPage = findPage(*currentPool, memoryBlock);

    /*  Pointer does not belong to us   */
    if (!currentPage) {// ⚠️ should never happen (pointer does not belong to us)
        return (changSize(ptr, oldSize, newSize, size));
    }

    /*  Pointer is last in page */
    if ((void *)(currentPage->data + currentPage->size) == memoryBlock + GET_SIZE(memoryBlock)) {
        return (changSize(ptr, oldSize, newSize, size));
    }

    void        *nextBlock      = NEXT_BLOCK(memoryBlock);
    bool        nextIs_used     = GET_FLAG(nextBlock);

    /*  Pointer have no room (next block is used)   */
    if (nextIs_used) {
        return (changSize(ptr, oldSize, newSize, size));
    }

    mergeBlocks(nextBlock, currentPage);

    /*  Pointer have place to expand    */
    if (GET_SIZE(nextBlock) + GET_SIZE(memoryBlock) >= newSize) {
        insertBlockPageHinted(currentPage, newSize, memoryBlock);
        return (ptr);
    }

    if (newSize < oldSize) {
        return (ptr);
    }
    void *new = ft_malloc(size);
    memcpy(new, ptr, size);
    ft_free(ptr);

    return new;
}
