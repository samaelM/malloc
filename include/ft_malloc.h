#ifndef FT_MALLOC_42
#   define FT_MALLOC_42

#   include "../libft/libft.h"

#   include <stddef.h>
#   include <stdlib.h>
#   include <unistd.h>
#   include <string.h>

#   include <sys/mman.h>
#   include <sys/types.h>
#   include <sys/stat.h>
#   include <fcntl.h>
#   include <unistd.h>
#   include <stdio.h>
#   include <curses.h>

#   define RED                  "\033[0;31m"
#   define GREEN                "\033[0;32m"
#   define YELLOW               "\033[0;33m"
#   define WHITE                "\033[0m"
#   define BOLD                 "\033[1m"
#   define FLASH                "\033[6m"
#   define RETURN               "\033[2K\r"

// #   define DEBUG(str)           printf( "%s: %sDEBUG:%s %s\n", __func__, YELLOW, WHITE, str)
#   define DEBUG(str)           // switch to this one to disable debug prints

#   define ALIGN(size)          (((size) + 15) & ~15)
#   define GET_SIZE(ptr)        ((*(size_t *)ptr >> 3) << 3)
#   define GET_FLAG(ptr)        (*(size_t *)ptr & 1)
#   define GET_BLOCK(ptr)       (ptr - sizeof(size_t))
#   define GET_PTR(ptr)         (ptr + sizeof(size_t))
#   define FLAG_FLIP(number)    (number ^ 1)
#   define NEXT_BLOCK(ptr)      (ptr + GET_SIZE(ptr))

#   define PAGESIZE             sysconf(_SC_PAGE_SIZE)
#   define TINY_POOL_SIZE       PAGESIZE
#   define TINY_POOL_ACCEPT     1024
#   define SMALL_POOL_SIZE      16 * PAGESIZE
#   define SMALL_POOL_ACCEPT    160384
#   define MIN_SPLIT_SIZE (sizeof(size_t) + 16)

typedef struct t_pool {
    size_t          size;
    struct t_pool   *next;
    char            _pad[8];    // padding to make data "aligned - 8"
    char            data[];
}   pool_t;

typedef struct t_global_pool {
    pool_t   *tinyPool;      // from 0 to 1024
    pool_t   *smallPool;     // from 1024 to 16k
    pool_t   *largePool;     // from 16k to infinite (one block per page)
}   gPool_t;

// POOLS
extern gPool_t g_pools;

// POOL MANIPULATION
void    *insertBlockPool(pool_t *pool, const size_t header);
int     poolAddBack(pool_t **pool, const size_t size);
pool_t  *poolLast(pool_t *pool);
pool_t  **findPool(size_t size);
void    deletePage(pool_t **pool, pool_t *page);
pool_t  *findPage(pool_t *pool, void *addr);
bool    isEmptyPage(pool_t *pool);
pool_t  **findPool(size_t size);
void    mergeBlocks(char *current, pool_t *pool);
void    *insertBlockPageHinted(pool_t *pool, const size_t header, void *hint);

// ALLOCATION
void    free(void *ptr);
void    *malloc(size_t size);
void    *realloc(void *ptr, size_t size);
void    show_alloc_mem();

#endif
