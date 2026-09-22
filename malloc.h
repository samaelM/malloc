#include <stdlib.h>

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define WHITE "\033[0m"
#define BOLD "\033[1m"
#define FLASH "\033[6m"
#define RETURN "\033[2K\r"

void free(void *ptr);
void *ft_malloc(size_t size);
void *realloc(void *ptr, size_t size);
void show_alloc_mem();
