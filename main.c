#include <stdio.h>
#include <string.h>
#include <libft.h>
#include "malloc.h"
#include <curses.h>


int main() {
    char    *ptr1;
    char    *ptr2;

    char    hello[13]   = "hello World!";
    char    mael[12]    = "hello Mael!";

    char    buffer[1024];

    printf(GREEN "ALLOCATION:\n" WHITE);
    {
	    ptr1 = ft_malloc(sizeof(hello));
	    if (!ptr1) {
	    	printf("ptr1 (%s bytes): " RED "FAIL\n" WHITE, ft_itoa_buffer(buffer, sizeof(hello)));
	    	return (1);
	    }
        printf("ptr1 (%s bytes): " GREEN "OK\n", ft_itoa_buffer(buffer, sizeof(hello)));
	    ptr2 = ft_malloc(sizeof(mael));
	    if (!ptr2) {
	    	printf("ptr2 (%s bytes): " RED "FAIL\n" WHITE, ft_itoa_buffer(buffer, sizeof(hello)));
	    	return (1);
	    }
        printf("ptr2 (%s bytes): " GREEN "OK\n", ft_itoa_buffer(buffer, sizeof(mael)));
    }
    printf(GREEN "TESTS PASSED\n\n" WHITE);

    printf(GREEN "WRITING:\n" WHITE);
    {
        memcpy(ptr1, hello, 13);
        printf("ptr1: " GREEN "OK\n" WHITE);
        memcpy(ptr2, mael, 12);
        printf("ptr2: " GREEN "OK\n" WHITE);
    }
    printf(GREEN "TESTS PASSED\n\n" WHITE);

    printf(GREEN "INTEGRITY:\n" WHITE);
    {
        const bool integrity_1 = strcmp(hello, ptr1) == 0;
        if (integrity_1) {
            printf("ptr1: " GREEN "OK\n" WHITE);
        } else {
            printf("ptr1 :\"%s\"\nstr :\"%s\"\n", ptr1, hello);
            printf("ptr1: " RED "FAIL\n" WHITE);
            return (1);
        }
        
        const bool integrity_2 = strcmp(mael, ptr2) == 0;
        if (integrity_2) {
            printf("ptr2: " GREEN "OK\n" WHITE);
        } else {
            printf("ptr2 :\"%s\"\nstr :\"%s\"\n", ptr2, mael);
            printf("ptr2: " RED "FAIL\n" WHITE);
            return (1);
        }
    }
    printf(GREEN "TESTS PASSED\n\n" WHITE);

    printf(GREEN "MEMORY:\n" WHITE);
    {
        ft_show_alloc_mem();
    }
	return (0);
}
