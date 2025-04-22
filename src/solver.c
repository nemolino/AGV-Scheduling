#include "solver.h"
#include "state.h"

#include <stdio.h>
#include <time.h>
    
void solver_run(Instance* ins)
{
    clock_t tic = clock();
    printf("... solver_run START\n");

    int J = ins->J;
    int W = ins->W;
    int U = ins->U;
    int* p = ins->p;
    int** t = ins->t;
    init_globals(ins);

    int best_obj_val = U;
    State* best_state = NULL;

    State* initial_state = state_create();
    state_set_count_not_started(initial_state, J);

    /*

    while (!pool.isEmpty()){

        // S := pool.popUnchecked()



		if S.IsFinal() {
			if S.t < bestObjVal {
				bestObjVal = S.t
				bestState = S
			}
			continue
		}

    } 
    */

    state_print(initial_state);
    state_destroy(initial_state);

    
    clock_t toc = clock();
    printf("... solver_run END\n");
    printf("solver_run execution time = %f s\n", (double)(toc - tic) / CLOCKS_PER_SEC);
}