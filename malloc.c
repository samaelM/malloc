#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#define DEBUG(str) printf("DEBUG: %s\n", str)

void *malloc(size_t size) {
	if (size == 0)
		return (NULL);
	void *memory;
	size_t pageSize = sysconf(_SC_PAGE_SIZE);
	int fd = open(".", O_RDWR | __O_TMPFILE);
	memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, fd, 0  * pageSize);
	close(fd);
	if (memory == MAP_FAILED) {
		DEBUG("MAP_FAILED");
		return (NULL);
	}
	return (memory);
}