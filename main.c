#include <stdio.h>
#include <string.h>
#include "malloc.h"

int main()
{
	char hello[13] = "hello world!";
	char *ptr = ft_malloc(13);
	if (!ptr) {
		printf("NULL\n");
		return 1;
	}
	memcpy(ptr, hello, 13);
	printf("%s", ptr);
	return 0;
}