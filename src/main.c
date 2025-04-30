#include "instance.h"
#include "solver.h"

int main(void) 
{
    Instance* ins = instance_create("../dataset_J_equals_M/J_12_M_12_r_2.1_seed_00.dat");
    instance_print(ins);
    printf("\n");
    solver_run(ins);
    printf("\n");
    instance_destroy(ins);
    return 0;
}