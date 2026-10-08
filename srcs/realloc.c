#include <ft_malloc.h>

void *realloc(void *ptr, size_t size) {
    if (!ptr) return malloc(size);

    if (size == 0) {
        free(ptr);
        return NULL;
    }

    void *memoryBlock = GET_BLOCK(ptr);
    size_t oldBlockSize = GET_SIZE(memoryBlock);
    size_t oldSize = oldBlockSize - sizeof(size_t);
    size_t newBlockSize = ALIGN(size + sizeof(size_t));

    if (oldBlockSize >= newBlockSize) {
        return ptr;
    }

    void *new = malloc(size);
    if (!new) return NULL;

    size_t len = oldSize < size ? oldSize : size;
    memcpy(new, ptr, len);
    free(ptr);

    return new;
}
