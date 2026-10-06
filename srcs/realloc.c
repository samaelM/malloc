#include <ft_malloc.h>


void    *ft_realloc(void *ptr, size_t size) {
    if (!ptr) {
        // DEBUG("realloc fail 1");
        void *new = ft_malloc(size);
        return new;
    }

    // get the memory block position (with header)
    void        *memoryBlock = GET_BLOCK(ptr);

    pool_t      **currentPool = findPool(GET_SIZE(memoryBlock));
    if (currentPool == &largePool) {
        // DEBUG("realloc fail 2");
        return NULL;
    }
    pool_t      *currentPage = findPage(*currentPool, memoryBlock);
    if (!currentPage) {
        // DEBUG("realloc fail 3");
        void *new = ft_malloc(size);
        memcpy(new, ptr, GET_SIZE(memoryBlock) - sizeof(size_t));
        ft_free(ptr);
        return new;
    }

    //last block
    if ((void *)(currentPage->data + currentPage->size) <= memoryBlock + GET_SIZE(memoryBlock)) {
        // DEBUG("realloc fail 4");
        void *new = ft_malloc(size);
        memcpy(new, ptr, GET_SIZE(memoryBlock) - sizeof(size_t));
        ft_free(ptr);
        return new;
    }
    void        *nextBlock   = NEXT_BLOCK(memoryBlock);
    bool        nextIs_used = GET_FLAG(nextBlock);
    if (nextIs_used) {
        // DEBUG("realloc fail 5");
        void *new = ft_malloc(size);
        memcpy(new, ptr, GET_SIZE(memoryBlock) - sizeof(size_t));
        ft_free(ptr);
        return new;
    }

    mergeBlocks(nextBlock, currentPage);
    if (GET_SIZE(nextBlock) + GET_SIZE(memoryBlock) >= ALIGN(size + sizeof(size_t))) {
        // ft_free(ptr);
        // printf("previous size = %ld, new size= %ld\n", GET_SIZE(memoryBlock), ALIGN(size + sizeof(size_t)));
        insertBlockPoolHinted(currentPage, ALIGN(size + sizeof(size_t)), memoryBlock);
        return ptr;
    }
    // DEBUG("realloc fail 6");
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