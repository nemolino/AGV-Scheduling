#include "solver.h"
    
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

    Pool* pool = pool_create(U);
    pool_add_initial_state(pool, initial_state);

    while (!pool_is_empty(pool)){

        State* s = pool_pop(pool); 

        // printf("popped state : ");
        // state_print(s);

        if (state_is_final(s)){
            if (state_get_t(s) < best_obj_val){
                best_obj_val = state_get_t(s);
                best_state = s;
            } else {
                // ... lo stato è aggiunto ai free
            }
            continue;
        }

        for (
            int k = J-1-state_get_count_finished(s); 
            k >= max(0, state_get_count_not_started(s)-1); 
            k--
        ) {
            // trying to move towards job k

            //	grazie a k := J - 1 - S.countFinished :
			//		non mi muovo mai verso il serbatoio finale, cioè ho sempre dest != M+1
			// 	grazie a k >= max(0, S.countNotStarted-1)
			// 		mi muovo verso il serbatoio iniziale al massimo 1 volta, cioè ho dest == 0 al massimo 1 volta

            int dest = state_get_xk(s,k);

            // mi muovo solo verso macchine con la dx libera o con a dx il serbatoio finale
			if (dest+1 != W+1 && state_is_xk_busy(s, dest+1)) {
				continue;
			}


        }
        
        
    }
    
    state_destroy(initial_state);

    pool_free(pool);
    
    clock_t toc = clock();
    printf("... solver_run END\n");
    printf("solver_run execution time = %f s\n", (double)(toc - tic) / CLOCKS_PER_SEC);
}

