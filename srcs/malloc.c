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
    //
    *(size_t *)memory->data = size;
    return (memory);
}

void mergeBlocks(char *current) {
    size_t size = (*(size_t *)current >> 3) << 3;
    char *next = current + size;
    bool is_used_next = *(size_t *)next & 1;
    while (!is_used_next) {
        size = (*(size_t *)next >> 3) << 3;
        if (size == 0) { // if it's the last block?// remplacer par next == pool.data+pool.size
            // printf("last block?%p\n", next);
            return;
        }
        *(size_t *)current += size;
        DEBUG("merged block");
        next = next + size;
        is_used_next = *(size_t *)next & 1;
    }
}

static inline void *insertHeader(pool_t *pool, size_t header) {
    if (!pool) return (NULL);
    char *current = pool->data;
    int safeIndex = 0; // 67
    while (current < pool->data + pool->size) {
        safeIndex++;
        if (safeIndex > 5) return (DEBUG("merge safeIndex"),NULL);
        bool is_used = *(size_t *)current & 1;
        if (!is_used) {
            mergeBlocks(current);
        }
        size_t size = (*(size_t *)current >> 3) << 3;

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
    // "realloc" the pool to make more space
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

static pool_t *tinyPool;

void *ft_malloc(size_t size) {
    if (size == 0)
        return (NULL);

    pool_t **currentPool = NULL;
    void *pointer = NULL;
    size_t realSize = ALIGN(size + sizeof(size_t));

    // selecting the best pool for size
    if (realSize <= 128) {//should add more pools for different sizes
        currentPool = &tinyPool;
    }

    pointer = insertHeader(*currentPool, realSize);
    if (!pointer) {
        poolAddBack(currentPool, 256);
        pointer = insertHeader(poolLast(*currentPool), realSize);
    }

    return (pointer);
}

void ft_free(void *ptr) {
    DEBUG("memory freed");
    *(size_t *)(ptr - sizeof(size_t))-=1;
    // if pool is empty, munmap it
}

void ft_show_alloc_mem() {
    pool_t *currentPool = tinyPool;
    printf(GREEN "TINY : %p\n" WHITE, currentPool);
    short pageIndex = 0;
    while (currentPool) {
        char *currentBlock = tinyPool->data;
        printf(YELLOW "PAGE %d:\n" WHITE, pageIndex);
        while (currentBlock < tinyPool->data + tinyPool->size) {
            size_t size = (*(size_t *)currentBlock >> 3) << 3;
            bool is_used = *(size_t *)currentBlock & 1;
    
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
    // print the memory as hex dump
    // for (size_t i = 0; i < tinyPool.size; i++) {
    //     if (i % 16 == 0)
    //         printf("\n%p: ", tinyPool.data + i);
    //     printf("%02x ", (unsigned char)tinyPool.data[i]);
    // }
    printf("\n");
}
