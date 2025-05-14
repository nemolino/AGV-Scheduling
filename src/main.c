#include "instance.h"
#include "solver.h"

int main(void) 
{   
    
    float r[] = {0.1, 0.4, 0.8, 1.1, 1.4, 1.8, 2.1}; //, 2.5, 3.0, 3.5, 4.0;
    size_t r_len = 7;
    char filename[100];
    
    for (int jm = 4; jm <= 12; jm++){
        for (int r_idx = 0; r_idx < r_len; r_idx++){
            for (int seed = 0; seed < 5; seed++){
                sprintf(filename, "../dataset_J_equals_M/J_%02d_M_%02d_r_%.1f_seed_%02d.dat", jm, jm, r[r_idx], seed);
                printf("%s\n", filename);
                Instance* ins = instance_create(filename);
                // instance_print(ins);
                // printf("\n");
                solver_run(ins);
                printf("\n");
                instance_destroy(ins);
            } 
        }
    }

    return 0;
}