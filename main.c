#include <stdio.h>
#include <string.h>
#include "malloc.h"

int main() {
	char *ptr = ft_malloc(13);
	if (!ptr) {
		printf("NULL\n");
		return 1;
	}
	memcpy(ptr, "hello world!", 13);
	printf("%s\n", ptr);

	char *ptr2 = ft_malloc(12);
	if (!ptr2) {
		printf("NULL\n");
		return 1;
	}
	memcpy(ptr2, "hello mael!", 12);
	printf("%s\n", ptr2);

	printf("\n%s - %s\n", ptr, ptr2);
	return 0;
}
