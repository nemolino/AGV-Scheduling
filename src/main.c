#include "instance.h"
#include "solver.h"

int main(void) 
{   
    SolverResult* sr = solver_result_create();

    float r[] = {0.1, 0.4, 0.8, 1.1, 1.4, 1.8, 2.1}; //, 2.5, 3.0, 3.5, 4.0;
    //float r[] = {2.1, 2.5, 3.0, 3.5, 4.0};
    size_t r_len = 7;
    //size_t r_len = 5;
    char filename[100];
    
    for (int jm = 4; jm <= 12; jm++){
    //for (int jm = 10; jm <= 10; jm++){
        for (int r_idx = 0; r_idx < r_len; r_idx++){
            for (int seed = 0; seed < 5; seed++){

                sprintf(filename, "../dataset_J_equals_M/J_%02d_M_%02d_r_%.1f_seed_%02d.dat", jm, jm, r[r_idx], seed);
                printf("%s\n", filename);

                Instance* ins = instance_create(filename);
                printf("J %d W %d max_extensions %d trivial_UB %d\n\n", 
                    ins->J, ins->W, ins->max_number_of_extensions, ins->U);
                // instance_print(ins);
                // printf("\n");
                
                printf("DP_exact\n");
                solver_result_clear(sr);
                solver_run(sr,ins);
                solver_result_print(sr);
                printf("\n");

                int z_optimal = sr->z_optimal;

                for (int e=1; e < ins->max_number_of_extensions; e++){
                    printf("DP_heuristic e %d\n", e);
                    solver_result_clear(sr);
                    solver_heuristic_run(sr,ins,e);
                    solver_result_print(sr);
                    printf("\n");
                    assert (sr->z_optimal >= z_optimal);
                    if (sr->z_optimal == z_optimal){
                        break;
                    }
                }
                printf("*******************************************************************************\n\n");
                instance_destroy(ins);
            } 
        }
    }

    solver_result_destroy(sr);

    return 0;
}
