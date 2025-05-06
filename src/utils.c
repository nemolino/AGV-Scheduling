#include "utils.h"

int max(int x, int y) 
{
    return (x >= y) ? x : y;
}

void *safe_malloc(size_t __size)
{
    void *ptr = malloc(__size);
    if (ptr == NULL) {
        printf("ERROR malloc\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void *safe_calloc(size_t __count, size_t __size)
{
    void *ptr = calloc(__count, __size);
    if (ptr == NULL) {
        printf("ERROR calloc\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void *safe_realloc(void *__ptr, size_t __size)
{
    void *ptr = realloc(__ptr, __size);
    if (ptr == NULL) {
        printf("ERROR realloc\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void safe_free(void *__ptr)
{
    if (__ptr == NULL) {
        printf("ERROR free\n");
        exit(EXIT_FAILURE);
    }
    free(__ptr);
}