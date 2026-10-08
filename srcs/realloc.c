#include <ft_malloc.h>

void *ft_realloc(void *ptr, size_t size) {
    if (!ptr) return ft_malloc(size);

    if (size == 0) {
        ft_free(ptr);
        return NULL;
    }

    void *memoryBlock = GET_BLOCK(ptr);
    size_t oldBlockSize = GET_SIZE(memoryBlock);
    size_t oldSize = oldBlockSize - sizeof(size_t);
    size_t newBlockSize = ALIGN(size + sizeof(size_t));

    if (oldBlockSize >= newBlockSize) {
        return ptr;
    }

    void *new = ft_malloc(size);
    if (!new) return NULL;

    size_t len = oldSize < size ? oldSize : size;
    memcpy(new, ptr, len);
    ft_free(ptr);

    return new;
}
