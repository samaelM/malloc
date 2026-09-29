#include <ft_malloc.h>

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define DEBUG(str) printf("%sDEBUG:%s %s\n", YELLOW, WHITE, str)
#define ALIGN(size) (((size) + 15) & ~15)
#define PAGESIZE sysconf(_SC_PAGE_SIZE)

typedef struct t_pool {// add attribute for destructor so it can munmap when exiting program
    size_t size;
    char *data;
    struct t_pool *next;
}pool_t;

static inline void *initPool(size_t size) {
    size_t pageSize = sysconf(_SC_PAGE_SIZE);
    void *memory = mmap(NULL,
                        size,
                        PROT_READ | PROT_WRITE,
                        MAP_ANON | MAP_SHARED,
                        -1,
                        0  * pageSize);

    if (memory == MAP_FAILED) {
        DEBUG("MAP_FAILED");
        return (NULL);
    }
    *(size_t *)memory = size;
    return (memory);
}

static inline void *insertHeader(pool_t pool, size_t header){
    char *current = pool.data;
    while (current < pool.data + pool.size) {
        size_t size = (*(size_t *)current >> 3) << 3;
        char is_used = *(size_t *)current & 1;

        // If the block is used or the size available is too small.
        if (is_used || size < header) {
            DEBUG("block skip");
            current += size;
            if (size == 0) // if it's the last block?
                return NULL;
        } else {
            size_t spaceLeft = size;
            size = header | 1;
            if (spaceLeft > header) {
                *(size_t *)(current + header) = spaceLeft - header;
                // printf("next block is %ld bytes long\n", *(size_t *)(current + header));
            }
            *(size_t *)current = size;
            DEBUG("found space");
            return (current + sizeof(size_t));
        }
    }
    DEBUG("not enough space in pool");
    // "realloc" the pool to make more space // 67
    return (NULL);
}
static pool_t tinyPool;

void *ft_malloc(size_t size) {
    if (size == 0)
        return (NULL);

    size_t realSize = ALIGN(size + sizeof(size_t));
    // printf("%sattempting to allocate %ld bytes memory block.(real size=%ld)%s\n", RED, size, realSize, WHITE);
    void *pointer = NULL;
    if (realSize <= 128) {//should change to something like 16 and add more pools for different sizes
        if (!tinyPool.size) {
            DEBUG("new pool");
            tinyPool.size = PAGESIZE;
            tinyPool.data = initPool(PAGESIZE);//should take ptr to pool and size as arguments
        }
        pointer = insertHeader(tinyPool, realSize);
    }

    return (pointer);
}

void ft_free(void *ptr) {
    *(size_t *)(ptr - sizeof(size_t))-=1;
    // should merge unused blocks
    // 
}

void ft_show_alloc_mem() {
    printf("TINY : %p\n", &tinyPool);
    char *current = tinyPool.data;
    while (current < tinyPool.data + tinyPool.size) {
        size_t size = (*(size_t *)current >> 3) << 3;
        char is_used = *(size_t *)current & 1;

        if (is_used) {
            printf("%p - %p : %ld bytes (used)\n", current + sizeof(size_t), current + size, size);
            current += size;
        } else {
            if (size> 0) {
                printf("%p - %p : %ld bytes (unused)\n", current + sizeof(size_t), current + size, size);
                current += size;
            } else
                break;
        }
    }
    // print the memory as hex dump
    // for (size_t i = 0; i < tinyPool.size; i++) {
    //     if (i % 16 == 0)
    //         printf("\n%p: ", tinyPool.data + i);
    //     printf("%02x ", (unsigned char)tinyPool.data[i]);
    // }
    // printf("\n");
}
