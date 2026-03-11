#include <sys/mman.h>
#include <aio.h>
#include <fcntl.h>

#ifndef NULL
	#define NULL (void *)0
#endif

void *realloc(void *ptr, size_t size) {
	if (ptr == NULL) {
		return (NULL);
	}
	int fd = open(".", __O_TMPFILE | O_RDWR);
	void *memory = mmap(ptr,
						size,
						PROT_READ | PROT_WRITE,
						MAP_ANON | MAP_SHARED,
						fd,
						0);
	close(fd);
	// we may need to save memory allocation's data somewhere.
	if (ptr != memory) { // memory is not at the same location, we need to copy the data from the previous location to the new one and free the previous pointer.
		size_t ptrSize = 0; // we need to find ptr size to be able to copy it's content.
		memcpy(memory, ptr, ptrSize);
		free(ptr); // free not yet implemented
		return (memory);
	}
	return (memory);
}
