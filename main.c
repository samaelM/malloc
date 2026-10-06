#include <ft_malloc.h>
#include <curses.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

// Replace with custom prototypes if not shadowing standard libc symbols:
// void *ft_malloc(size_t size);
// void ft_free(void *ptr);
// void *ft_realloc(void *ptr, size_t size);

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            fprintf(stderr, "[FAIL] %s: %s (line %d)\n", __func__, msg, __LINE__); \
            ft_show_alloc_mem(); \
            return false; \
        } \
    } while (0)

#define PASS() \
    do { \
        printf("[PASS] %s\n", __func__); \
        return true; \
    } while (0)

/* ------------------------------------------------------------------------- */
/* 1. Alignment & Basic Payload Safety                                       */
/* ------------------------------------------------------------------------- */

// Check standard alignment requirements: 8-byte on 32-bit, 16-byte on 64-bit
bool test_alignment_and_integrity(void) {
    const size_t align = sizeof(void *) * 2;
    const size_t sizes[] = { 1, 3, 7, 8, 9, 15, 16, 24, 31, 64, 127, 256, 1024, 4096 };
    const size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    void *ptrs[sizeof(sizes) / sizeof(sizes[0])];

    for (size_t i = 0; i < num_sizes; ++i) {
        ptrs[i] = ft_malloc(sizes[i]);
        TEST_ASSERT(ptrs[i] != NULL, "ft_malloc returned NULL on valid size");
        TEST_ASSERT(((uintptr_t)ptrs[i] % align) == 0, "Returned pointer violates required byte alignment");

        // Fill payload completely to catch single-byte off-by-one errors against chunk headers
        memset(ptrs[i], (int)(0xAA + (i & 0x0F)), sizes[i]);
    }

    // Verify memory integrity across contiguous chunks
    for (size_t i = 0; i < num_sizes; ++i) {
        uint8_t expected = (uint8_t)(0xAA + (i & 0x0F));
        uint8_t *buf = (uint8_t *)ptrs[i];
        for (size_t b = 0; b < sizes[i]; ++b) {
            TEST_ASSERT(buf[b] == expected, "Heap corruption detected in payload area");
        }
        ft_free(ptrs[i]);
    }

    PASS();
}

/* ------------------------------------------------------------------------- */
/* 2. Standard Specification Edge Cases                                      */
/* ------------------------------------------------------------------------- */

bool test_posix_edge_cases(void) {
    // ft_malloc(0) may return NULL or a unique non-dereferenceable pointer that can be ft_freed
    void *zero_ptr = ft_malloc(0);
    if (zero_ptr != NULL) {
        ft_free(zero_ptr);
    }

    // ft_free(NULL) MUST perform no operation without faulting
    ft_free(NULL);

    // ft_realloc(NULL, size) MUST behave identically to ft_malloc(size)
    void *r_null = ft_realloc(NULL, 128);
    TEST_ASSERT(r_null != NULL, "ft_realloc(NULL, size) failed to allocate");
    memset(r_null, 0x55, 128);

    // ft_realloc(ptr, 0) should return NULL or a valid pointer to be ft_freed, ft_freeing the old chunk
    void *r_zero = ft_realloc(r_null, 0);
    if (r_zero != NULL) {
        ft_free(r_zero);
    }

    PASS();
}

/* ------------------------------------------------------------------------- */
/* 3. ft_Realloc Behavior: Shrink, Growth, and Data Preservation               */
/* ------------------------------------------------------------------------- */

bool test_ft_realloc_mechanics(void) {
    size_t initial_size = 64;
    uint8_t *ptr = (uint8_t *)ft_malloc(initial_size);
    TEST_ASSERT(ptr != NULL, "Initial ft_malloc failed");

    for (size_t i = 0; i < initial_size; ++i) {
        ptr[i] = (uint8_t)(i & 0xFF);
    }

    // Shrink in-place or preserve subset
    size_t shrink_size = 16;
    ptr = (uint8_t *)ft_realloc(ptr, shrink_size);
    TEST_ASSERT(ptr != NULL, "ft_realloc shrink returned NULL");
    for (size_t i = 0; i < shrink_size; ++i) {
        TEST_ASSERT(ptr[i] == (uint8_t)(i & 0xFF), "Shrunk buffer corrupted data prefix");
    }

    // Substantial expansion: forces relocation if neighboring chunks are occupied
    void *blocker = ft_malloc(128); // Block rightward in-place chunk expansion
    TEST_ASSERT(blocker != NULL, "Blocker allocation failed");

    size_t expand_size = 8192;
    ptr = (uint8_t *)ft_realloc(ptr, expand_size);
    TEST_ASSERT(ptr != NULL, "ft_realloc expansion failed");
    for (size_t i = 0; i < shrink_size; ++i) {
        TEST_ASSERT(ptr[i] == (uint8_t)(i & 0xFF), "Expanded buffer failed to preserve prior bytes");
    }

    // Ensure new trailing memory is writeable without segfaulting
    memset(ptr + shrink_size, 0xEE, expand_size - shrink_size);

    ft_free(ptr);
    ft_free(blocker);

    PASS();
}

/* ------------------------------------------------------------------------- */
/* 4. Chunk Coalescing & Fragmentation Check                                 */
/* ------------------------------------------------------------------------- */

// Verifies that adjacent ft_freed blocks merge and satisfy an allocation
// equal to the sum of their sizes without allocating new pages/mmap/sbrk.
bool test_coalescing(void) {
    const size_t chunk_size = 512;
    void *p1 = ft_malloc(chunk_size);
    void *p2 = ft_malloc(chunk_size);
    void *p3 = ft_malloc(chunk_size);

    TEST_ASSERT(p1 && p2 && p3, "Sequential allocations failed");

    // ft_Free all three blocks in an order that exercises left and right merging
    ft_free(p2); // Middle block ft_free
    ft_free(p1); // Coalesce right (p1 + p2)
    ft_free(p3); // Coalesce left ((p1 + p2) + p3)

    // If coalesced properly, requesting ~1.5x original chunk size can reuse this space
    void *reunited = ft_malloc(chunk_size * 2);
    TEST_ASSERT(reunited != NULL, "Failed to allocate in coalesced space");

    // If your allocator reuses ft_free lists in address order, reunited will match p1
    // (Informational check; depending on allocator design this might be equal)
    ft_free(reunited);

    PASS();
}

/* ------------------------------------------------------------------------- */
/* 5. Pseudorandom Stress & Interleaved Lifetime Test                        */
/* ------------------------------------------------------------------------- */

#define STRESS_SLOTS 256
#define STRESS_ITERS 1000

bool test_stress_churn(void) {
    struct Slot {
        uint8_t *ptr;
        size_t size;
        uint8_t pattern;
    } slots[STRESS_SLOTS] = {0};

    // Linear Congruential Generator (deterministic across runs)
    uint32_t lcg_state = 0x12345678;
    auto uint32_t next_rand(void) {
        lcg_state = lcg_state * 1664525u + 1013904223u;
        return lcg_state;
    }

    for (int iter = 0; iter < STRESS_ITERS; ++iter) {
        size_t idx = next_rand() % STRESS_SLOTS;

        if (slots[idx].ptr == NULL) {
            // Allocate new chunk with variable sizes (1 to 4096 bytes)
            size_t sz = 1 + (next_rand() % 4096);
            uint8_t pat = (uint8_t)(next_rand() & 0xFF);

            slots[idx].ptr = (uint8_t *)ft_malloc(sz);
            printf("malloc (%ld): %p\n", sz, GET_BLOCK(slots[idx].ptr));
            TEST_ASSERT(slots[idx].ptr != NULL, "Stress test: malloc failed");
            slots[idx].size = sz;
            slots[idx].pattern = pat;

            memset(slots[idx].ptr, pat, sz);
        } else {
            // 20% ft_realloc, 80% ft_free
            uint32_t action = next_rand() % 10;

            // Verify integrity of current buffer before mutating
            for (size_t b = 0; b < slots[idx].size; ++b) {
                // printf("%d ", slots[idx].ptr[b]);
                if (slots[idx].ptr[b] != slots[idx].pattern) {
                    // for (size_t b = 0; b < slots[idx].size; ++b) {
                    //     printf("%d ", slots[idx].ptr[b]);
                    // }
                    printf("\n[%d(%p)]`%d` - `%d`\n", idx, GET_BLOCK(slots[idx].ptr), slots[idx].ptr[b], slots[idx].pattern);
                }
                TEST_ASSERT(slots[idx].ptr[b] == slots[idx].pattern, "Stress test: data corrupted by neighboring writes");
            }
            printf("\n");

            if (action < 2) {
                // ft_Realloc
                size_t new_sz = 1 + (next_rand() % 4096);
                slots[idx].ptr = (uint8_t *)ft_realloc(slots[idx].ptr, new_sz);
                TEST_ASSERT(slots[idx].ptr != NULL, "Stress test: ft_realloc failed");

                // Update size and fill newly added bytes (if any)
                if (new_sz > slots[idx].size) {
                    memset(slots[idx].ptr + slots[idx].size, slots[idx].pattern, new_sz - slots[idx].size);
                }
                slots[idx].size = new_sz;
            } else {
                // ft_Free
                ft_free(slots[idx].ptr);
                slots[idx].ptr = NULL;
                slots[idx].size = 0;
            }
        }
    }

    // Cleanup remaining slots
    for (size_t i = 0; i < STRESS_SLOTS; ++i) {
        if (slots[i].ptr != NULL) {
            for (size_t b = 0; b < slots[i].size; ++b) {
                TEST_ASSERT(slots[i].ptr[b] == slots[i].pattern, "Final cleanup: corruption detected");
            }
            ft_free(slots[i].ptr);
            slots[i].ptr = NULL;
        }
    }

    PASS();
}

/* ------------------------------------------------------------------------- */
/* Main Runner                                                               */
/* ------------------------------------------------------------------------- */

int ttt(void) {
    printf("Starting custom allocator test suite...\n\n");

    bool ok = true;
    // ok = ok && test_alignment_and_integrity();
    // ok = ok && test_posix_edge_cases();
    // ok = ok && test_ft_realloc_mechanics();
    // ok = ok && test_coalescing();
    ok = ok && test_stress_churn();

    if (ok) {
        printf("\nAll test phases passed successfully.\n");
        return 0;
    }

    printf("\nTest suite aborted due to failure.\n");
    return 1;
}

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

//     printf(RED "MEMORY:\n" WHITE);
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


// int main() {
//     printf("ptr1: allocating 120 bytes\n");
//     char *ptr1 = ft_malloc(120);
//     ft_show_alloc_mem();
//     printf("ptr2: allocating 120 bytes\n");
//     char *ptr2 = ft_malloc(120);
//     ft_show_alloc_mem();
//     printf("ptr3: allocating 1024 bytes\n");
//     char *ptr3 = ft_malloc(1024);
//     printf("pointer 3: %p\n", ptr3);
//     ft_show_alloc_mem();
//     printf("ptr1: free\n");
//     if(ptr1) ft_free(ptr1);
//     printf("ptr2: free\n");
//     if(ptr2) ft_free(ptr2);
//     ft_show_alloc_mem();
//     printf("ptr3: free\n");
//     if(ptr3) ft_free(ptr3);
//     ft_show_alloc_mem();
//     return 0;
// }

int main() {
    // // printf("ptr1: allocating 120 bytes\n");
    // char *ptr1 = ft_malloc(120);
    // ft_show_alloc_mem();
    // // printf("ptr2: allocating 120 bytes\n");
    // char *ptr2 = ft_malloc(120);
    // ft_show_alloc_mem();
    // printf(RED "REALLOC\n" WHITE);
    // ptr1 = ft_realloc(ptr1, 240);
    // ft_show_alloc_mem();
    // ptr2 = ft_realloc(ptr2, 520);
    // ft_show_alloc_mem();

    // ft_free(ptr1);
    // ft_free(ptr2);

    ttt();
    return (1);
}
