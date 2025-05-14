#ifndef INSTANCE_H
#define INSTANCE_H

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

typedef struct {
    int     J;
    int     W;
    int*    p;
    int**   t;
    int     U;
    int     max_number_of_extensions;
} Instance;

Instance*   instance_create     (char* file_path);
void        instance_print      (Instance* ins);
void        instance_destroy    (Instance* ins);

#endif 