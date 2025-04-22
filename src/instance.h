#ifndef INSTANCE_H
#define INSTANCE_H

typedef struct {
    int J;
    int W;
    int* p;
    int** t;
    int U;
} Instance;

Instance*   instance_create     (char* file_path);
void        instance_print      (Instance* ins);
void        instance_destroy    (Instance* ins);

#endif 