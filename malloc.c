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

#define DEBUG(str) printf("DEBUG: %s\n", str)
#define ALIGN(size) (((size) + 15) & ~15)

typedef struct t_pool {// add attribute for destructor so it can munmap when exiting program
	size_t size;
	char *data;
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
		if (*(size_t *)current & 1 || *(size_t *)current < header) {
			DEBUG("BLOCK SKIP");
			current += *(size_t *)current;
		}
		else {
			size_t spaceLeft = *(size_t *)current;
			*(size_t *)current = header;
			*(current + header) = spaceLeft - header;
			DEBUG("found space");
			return (current += 1);
		}
	}
	DEBUG("not enough space in pool");
	// "realloc" the pool to make more space
	return (NULL);
}

void *ft_malloc(size_t size) {
	static pool_t tinyPool;
	if (size == 0)
		return (NULL);

	size_t realSize = ALIGN(size + sizeof(size_t));
	void *pointer = NULL;
	if (realSize < 46) {//should change to something like 16 and add more pools for different sizes
		if (!tinyPool.size) {
			tinyPool.size = 1000;
			tinyPool.data = initPool(1000);//should take ptr to pool and size as arguments
		}
		pointer = insertHeader(tinyPool, realSize);
	}
	
	return (pointer);
}


