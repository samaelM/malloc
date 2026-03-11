#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define DEBUG(str) printf("DEBUG: %s\n", str)

/*
We need to make some sort of storage to store the pointer and their sizes.
We also need to make pools of available memory to increase malloc's performance.
This pools need to divided in 3 categories: TINY, SMALL, and LARGE.
*/

void *malloc(size_t size) {
	if (size == 0)
		return (NULL);
	size_t pageSize = sysconf(_SC_PAGE_SIZE);
	int fd = open(".", O_RDWR | __O_TMPFILE);
	void *memory = mmap(NULL,
						size,
						PROT_READ | PROT_WRITE,
						MAP_ANON | MAP_SHARED,
						fd,
						0  * pageSize);
	close(fd);
	if (memory == MAP_FAILED) {
		DEBUG("MAP_FAILED");
		return (NULL);
	}
	return (memory);
}
