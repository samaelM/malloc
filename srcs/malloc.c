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
#include <curses.h>

#define DEBUG(str) printf("%sDEBUG:%s %s\n", YELLOW, WHITE, str)
#define ALIGN(size) (((size) + 15) & ~15)
#define GET_SIZE(ptr) ((*(size_t *)ptr >> 3) << 3)
#define GET_FLAG(ptr) (*(size_t *)ptr & 1)
#define GET_BLOCK(ptr) (ptr - sizeof(size_t))
#define PAGESIZE sysconf(_SC_PAGE_SIZE)

typedef struct t_pool {
    size_t size;
    struct t_pool *next;
    char data[];
}pool_t;

static inline pool_t *initPool(size_t size) {
    size_t pageSize = sysconf(_SC_PAGE_SIZE);
    pool_t *memory = mmap(NULL,
                        size,
                        PROT_READ | PROT_WRITE,
                        MAP_ANON | MAP_SHARED,
                        -1,
                        0  * pageSize);

    if (memory == MAP_FAILED) {
        DEBUG("MAP_FAILED");
        return (NULL);
    }
    memory->size = size - sizeof(size_t) - sizeof(pool_t *);
    printf("final pool size=%ld\n", memory->size);
    memory->next = NULL;
    *(size_t *)memory->data = size - sizeof(size_t) - sizeof(pool_t *);
    return (memory);
}

void mergeBlocks(char *current) {
    size_t size = GET_SIZE(current);
    char *next = current + size;
    bool is_used_next = GET_FLAG(next);
    while (!is_used_next) {
        size = GET_SIZE(next);
        if (size == 0) { // if it's the last block?// remplacer par next == pool.data+pool.size
            // printf("last block?%p\n", next);
            return;
        }
        *(size_t *)current += size;
        DEBUG("merged block");
        next = next + size;
        is_used_next = GET_FLAG(next);
    }
}

static inline void *insertHeader(pool_t *pool, size_t header) {
    if (!pool) return (NULL); // 67
    char *current = pool->data;
    int safeIndex = 0;
    while (current < pool->data + pool->size) {
        safeIndex++;
        if (safeIndex > 5) return (DEBUG("merge safeIndex"),NULL);
        bool is_used = GET_FLAG(current);
        if (!is_used) {
            mergeBlocks(current);
        }
        size_t size = GET_SIZE(current);

        // If the block is used or the size available is too small.
        if (is_used || size < header) {
            DEBUG("block skip");
            current += size;
            if (size == 0) { // if it's the last block?
                DEBUG("ohoh problemo");
                return NULL;
            }
        } else {
            size_t spaceLeft = size;
            size = header | 1;
            if (spaceLeft > header) {
                *(size_t *)(current + header) = spaceLeft - header;
            }
            *(size_t *)current = size;
            DEBUG("found space");
            return (current + sizeof(size_t));
        }
    }
    DEBUG("not enough space in pool");
    return (NULL);
}

pool_t *poolLast(pool_t *pool) {
    if (!pool) return (NULL);

    while (pool->next) {
        pool = pool->next;
    }
    return (pool);
}

int poolAddBack(pool_t **pool, size_t size) {
    pool_t *new = initPool(size);
    if (new == NULL) return (0);
    DEBUG("new pool");

    if (!*pool){
        *pool = new;
    }else {
        poolLast(*pool)->next = new;
    }
    return (1);
}

static pool_t *tinyPool; // from 0 to 1024
static pool_t *smallPool; // from 1024 to 16k

static pool_t *largePool; // from 16k to infinite (one block per page)

void *ft_malloc(size_t size) {
    if (size == 0)
        return (NULL);

    pool_t **currentPool;
    void *pointer;
    size_t realSize = ALIGN(size + sizeof(size_t));
    size_t poolSize = PAGESIZE;

    // selecting the best pool for size
    if (realSize <= 1024) currentPool = &tinyPool;
    else if (realSize <= 160384) {
        currentPool = &smallPool;
        poolSize = 16 * PAGESIZE;
    } else {
        currentPool = &largePool;
        poolSize = realSize + sizeof(pool_t *) + sizeof(size_t);
    }

    // insert the header in the pools
    pointer = insertHeader(*currentPool, realSize);
    if (!pointer) {
        poolAddBack(currentPool, poolSize);
        pointer = insertHeader(poolLast(*currentPool), realSize);
    }

    return (pointer);
}

void ft_free(void *ptr) {
    if (!ptr) return;
    DEBUG("memory freed");
    // get the memory block position (with header)
    void *memoryBlock = GET_BLOCK(ptr);

    // fliping the is_used flag
    *(size_t *)memoryBlock -= 1;

    // if pool page is empty, munmap it, and don't break the chain

    // get the size of the block to get pool
    // size_t size = (*(size_t *)memoryBlock >> 3) << 3;
    // find the owner of the pointer

}

void printPool(pool_t *pool, char *name) {
    pool_t *currentPool = pool;
    printf(GREEN "%s : %p\n" WHITE, name, currentPool);
    short pageIndex = 0;
    while (currentPool) {
        char *currentBlock = currentPool->data;
        printf(YELLOW "PAGE %d: %ld bytes\n" WHITE, pageIndex, currentPool->size);
        while (currentBlock < currentPool->data + currentPool->size) {
            size_t size = GET_SIZE(currentBlock);
            bool is_used = GET_FLAG(currentBlock);
    
            if (is_used) {
                printf("\t%p - %p : %ld bytes (used)\n", currentBlock, currentBlock + size, size);
                currentBlock += size;
            } else {
                if (size> 0) {
                    printf("\t%p - %p : %ld bytes (unused)\n", currentBlock, currentBlock + size, size);
                    currentBlock += size;
                } else
                    break;
            }
        }
        currentPool = currentPool->next;
        pageIndex++;
    }
    printf("\n");
}

void ft_show_alloc_mem() {
    printPool(tinyPool, "TINY");
    printPool(smallPool, "SMALL");
    printPool(largePool, "LARGE");
}

// print the memory as hex dump
// for (size_t i = 0; i < tinyPool.size; i++) {
//     if (i % 16 == 0)
//         printf("\n%p: ", tinyPool.data + i);
//     printf("%02x ", (unsigned char)tinyPool.data[i]);
// }
