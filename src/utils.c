#include "utils.h"

int max(int x, int y) 
{
    return (x >= y) ? x : y;
}

void *safe_malloc(size_t __size)
{
    void *ptr = malloc(__size);
    if (ptr == NULL) {
        printf("Memory allocation with malloc failed!\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void *safe_calloc(size_t __count, size_t __size)
{
    void *ptr = calloc(__count, __size);
    if (ptr == NULL) {
        printf("Memory allocation with calloc failed!\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}