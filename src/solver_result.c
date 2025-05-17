#include "solver.h"

SolverResult* solver_result_create ()
{
    SolverResult* sr = (SolverResult*)safe_malloc(sizeof(SolverResult));
    sr->extensions_from_each_state_count = (int*)safe_malloc(100 * sizeof(int));
    solver_result_clear(sr);
    return sr;
}

void solver_result_destroy (SolverResult* sr)
{
    safe_free(sr->extensions_from_each_state_count);
    sr->extensions_from_each_state_count = NULL;
    safe_free(sr);
    sr = NULL;
}

void solver_result_clear (SolverResult* sr)
{
    sr->z_optimal = -1;
    sr->execution_time = -1;
    sr->explored_states_count = -1;
    for (int i=0; i < 100; i++){
        sr->extensions_from_each_state_count[i] = -1;
    }
    sr->push_success_count = -1;
    sr->push_fail_count = -1;
    sr->bitset_used_size = -1;
    sr->state_allocations_count = -1;
}

void solver_result_print (SolverResult* sr)
{
    printf("z %d t %.6f extensions ", sr->z_optimal, sr->execution_time);
    for (int i=0; i < 100; i++){
        if (sr->extensions_from_each_state_count[i] == -1){
            break;
        } else{
            printf("%d,", sr->extensions_from_each_state_count[i]);
        }
    }
    printf(" push_succ %d push_fail %d bitset_used_size %d state_allocations_count %d\n",
        sr->push_success_count, sr->push_fail_count, sr->bitset_used_size, sr->state_allocations_count
    );
    
}