#include <ft_malloc.h>


void    *ft_realloc(void *ptr, size_t size) {
    DEBUG("--- entering realloc ---");
    if (!ptr) {
        void *new = ft_malloc(size);

        return (new);
    }

    void        *memoryBlock = GET_BLOCK(ptr);
    pool_t      **currentPool = findPool(GET_SIZE(memoryBlock));

    if (currentPool == &largePool) return (NULL);
    pool_t      *currentPage = findPage(*currentPool, memoryBlock);

    if (!currentPage) {
        void *new = ft_malloc(size);
        memcpy(new, ptr, GET_SIZE(memoryBlock) - sizeof(size_t));
        ft_free(ptr);

        return (new);
    }

    if ((void *)(currentPage->data + currentPage->size) <= memoryBlock + GET_SIZE(memoryBlock)) {
        void *new = ft_malloc(size);
        memcpy(new, ptr, GET_SIZE(memoryBlock) - sizeof(size_t));
        ft_free(ptr);

        return (new);
    }

    void        *nextBlock      = NEXT_BLOCK(memoryBlock);
    bool        nextIs_used     = GET_FLAG(nextBlock);

    if (nextIs_used) {
        void *new = ft_malloc(size);
        memcpy(new, ptr, GET_SIZE(memoryBlock) - sizeof(size_t));
        ft_free(ptr);
        return (new);
    }

    mergeBlocks(nextBlock, currentPage);

    if (GET_SIZE(nextBlock) + GET_SIZE(memoryBlock) >= ALIGN(size + sizeof(size_t))) {
        insertBlockPageHinted(currentPage, ALIGN(size + sizeof(size_t)), memoryBlock);
        return (ptr);
    }

    void *new = ft_malloc(size);
    memcpy(new, ptr, GET_SIZE(memoryBlock) - sizeof(size_t));
    ft_free(ptr);

    return new;
}


/*
realloc:
    currentBlock = (ptr-header);
    next = ptr + GET_SIZE(currentBlock)
    is_used = GET_FLAG(next)
    if (is_used) return
    mergeBlocks(next)
    if (GET_SIZE(next) + GET_SIZE(currentBlock) >= ALIGN(size+header)) {
        free(ptr);
        insertHeader(pool, realsize) // how to get pool??? with realsize?
    }
*/