#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>

int max(int x, int y);

void *safe_malloc   (size_t __size);
void *safe_calloc   (size_t __count, size_t __size);
void *safe_realloc  (void *__ptr, size_t __size);

#endif 