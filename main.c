#include <stdio.h>
#include <string.h>
#include <ft_malloc.h>
#include <curses.h>


// int main() {
//     char    *ptr1;
//     char    *ptr2;

//     char    hello[13]   = "hello World!";
//     char    mael[12]    = "hello Mael!";

//     printf(GREEN "ALLOCATION:\n" WHITE);
//     {
// 	    ptr1 = ft_malloc(sizeof(hello));
// 	    if (!ptr1) {
// 	    	printf("ptr1 (%ld bytes): " RED "FAIL\n" WHITE, sizeof(hello));
// 	    	return (1);
// 	    }
//         printf("ptr1 (%ld bytes): " GREEN "OK\n", sizeof(hello));
// 	    ptr2 = ft_malloc(sizeof(mael));
// 	    if (!ptr2) {
// 	    	printf("ptr2 (%ld bytes): " RED "FAIL\n" WHITE, sizeof(hello));
// 	    	return (1);
// 	    }
//         printf("ptr2 (%ld bytes): " GREEN "OK\n", sizeof(mael));
//     }
//     printf(GREEN "TESTS PASSED\n\n" WHITE);

//     printf(GREEN "WRITING:\n" WHITE);
//     {
//         memcpy(ptr1, hello, 13);
//         printf("ptr1: " GREEN "OK\n" WHITE);
//         memcpy(ptr2, mael, 12);
//         printf("ptr2: " GREEN "OK\n" WHITE);
//     }
//     printf(GREEN "TESTS PASSED\n\n" WHITE);

//     printf(GREEN "INTEGRITY:\n" WHITE);
//     {
//         const bool integrity_1 = strcmp(hello, ptr1) == 0;
//         if (integrity_1) {
//             printf("ptr1: " GREEN "OK\n" WHITE);
//         } else {
//             printf("ptr1 :\"%s\"\nstr :\"%s\"\n", ptr1, hello);
//             printf("ptr1: " RED "FAIL\n" WHITE);
//             return (1);
//         }
        
//         const bool integrity_2 = strcmp(mael, ptr2) == 0;
//         if (integrity_2) {
//             printf("ptr2: " GREEN "OK\n" WHITE);
//         } else {
//             printf("ptr2 :\"%s\"\nstr :\"%s\"\n", ptr2, mael);
//             printf("ptr2: " RED "FAIL\n" WHITE);
//             return (1);
//         }
//     }
//     printf(GREEN "TESTS PASSED\n\n" WHITE);

//     printf(GREEN "MEMORY:\n" WHITE);
//     {
//         ft_show_alloc_mem();

//         char * ptr3 = ft_malloc(16);
//         ft_show_alloc_mem();

//         ft_free(ptr1);
//         ft_show_alloc_mem();

//         ft_free(ptr2);
//         ft_show_alloc_mem();

//         char * ptr4 = ft_malloc(32);
//         ft_show_alloc_mem();
//         ft_free(ptr3);
//         ft_free(ptr4);
//         ft_show_alloc_mem();
//         ptr1 = ft_malloc(1);
//         ft_show_alloc_mem();
//         ft_free(ptr1);

//     }
//     printf(GREEN FLASH "IF YOU SEE THIS MESSAGE I THINK IT'S GOOD!\n" WHITE);
// 	return (0);
// }


int main() {
    printf("ptr1: allocating 120 bytes\n");
    char *ptr1 = ft_malloc(120);
    ft_show_alloc_mem();
    printf("ptr2: allocating 120 bytes\n");
    char *ptr2 = ft_malloc(120);
    ft_show_alloc_mem();
    printf("ptr3: allocating 120 bytes\n");
    char *ptr3 = ft_malloc(120);
    ft_show_alloc_mem();
    printf("ptr1: free\n");
    if(ptr1) ft_free(ptr1);
    printf("ptr2: free\n");
    if(ptr2) ft_free(ptr2);
    printf("ptr3: free\n");
    if(ptr3) ft_free(ptr3);
    ft_show_alloc_mem();
    return 0;
}
