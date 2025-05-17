#include "solver.h"

int LIMIT;
int J;
int W;

// void print_optimal_solution (State* best_state)
// {
//     printf("print_optimal_solution ...\n");
//     State* cur = best_state;
//     do {
//         state_print(cur);
//         cur = cur->pred;
//     } while(cur != NULL);
// }

void solver_heuristic_run (SolverResult* sr, Instance* ins, int extensions_threshold)
{
    assert (extensions_threshold >= 1 && extensions_threshold <= ins->max_number_of_extensions);
    //printf("solver_run e = %d\n", extensions_threshold);

    // global variables
    J = ins->J;
    W = ins->W;
    LIMIT = 0; 
    
    sr->explored_states_count = 0;
    for (int i=0; i <= ins->max_number_of_extensions; i++){
        sr->extensions_from_each_state_count[i] = 0;
    }
    sr->push_success_count = 0;
    sr->push_fail_count = 0;

    HeuristicExtension* he = heuristic_extension_create(
        ins->max_number_of_extensions, extensions_threshold
    );
    
    StateAllocator* allocator = state_allocator_create();

    clock_t tic = clock();

    int best_obj_val = ins->U;
    State* best_state = NULL;

    Pool* pool = pool_create(ins->U);

    State* initial_state = state_create(allocator);
    initial_state->count_not_started = J;
    pool_try_push(pool, allocator, initial_state);

    while (!pool_is_empty(pool)){

        State* s = pool_pop(pool);

        sr->explored_states_count++;

        // printf("popped state : ");
        // state_print(s);
        
        // il primo stato finale che raggiungo è la soluzione ottima, 
        // perchè estraggo gli stati dal pool in ordine di tempo crescente
        if (state_is_final(s)){
            //assert (s->t <= best_obj_val);
            //printf("SOLUZIONE OTTIMA TROVATA : stato finale raggiunto\n");
            //if (s->t < best_obj_val){
            best_obj_val = s->t;
            best_state = s;
            //}
            break;
        }

        heuristic_extension_clear(he);
        
        for (int k = J-1-(s->count_finished); k >= max(0, s->count_not_started-1); k--) {

            // trying to move towards job k

            //	grazie a k := J - 1 - S.countFinished :
			//		non mi muovo mai verso il serbatoio finale, cioè ho sempre dest != M+1
			// 	grazie a k >= max(0, S.countNotStarted-1)
			// 		mi muovo verso il serbatoio iniziale al massimo 1 volta, cioè ho dest == 0 al massimo 1 volta

            int dest = (s->xk)[k];

            // mi muovo solo verso macchine con la dx libera o con a dx il serbatoio finale
			if (dest+1 != W+1 && state_is_workstation_busy(s, dest+1)) {
                continue;
			}

            // passa il tempo di spostamento
            // prima di muovere il job bisogna aspettare che la sua lavorazione finisca
            heuristic_extension_add(
                he, k, max(s->t + (ins->t)[s->x][dest], (s->ek)[k])
            );
        }

        // controllo invarianti
        assert (he->k_cur_size >= 1 && he->k_cur_size <= he->extensions_max);
        for (int i=he->k_cur_size; i < he->extensions_max; i++){
            assert (
                he->k[i] == -1 && he->k_times[i] == -1
            );
        }

        heuristic_extension_calculate_best_extensions(he);

        int ref_count = 0;
        int number_of_extensions = 0;
        for (int idx = 0; idx < he->best_k_cur_size; idx++) {

            int k = he->best_k[idx];
            int dest = (s->xk)[k];
            
            number_of_extensions++;

            State* s_next = state_create(allocator);//state_get_or_create(poolfree);

            // passa il tempo di spostamento
            // s_next->t = s->t + (ins->t)[s->x][dest];
            
            // prima di muovere il job bisogna aspettare che la sua lavorazione finisca
            // s_next->t = max(s_next->t, (s->ek)[k]);

            s_next->t = he->k_times[idx];

            // muovo il job verso la macchina successiva
            // passa il tempo di spostamento
            s_next->t += (ins->t)[dest][dest+1];
            s_next->x = dest+1;

            // for (int i = 0; i < J; i++) {
            //     (s_next->xk)[i] = (s->xk)[i];
            //     (s_next->ek)[i] = (s->ek)[i];
            // }
            memcpy(s_next->xk, s->xk, J * sizeof(int));
            memcpy(s_next->ek, s->ek, J * sizeof(int));

            (s_next->xk)[k]++;
            (s_next->ek)[k] = (s_next->x == W+1) ? -1 : s_next->t + (ins->p)[s_next->x];

            s_next->count_not_started = (s_next->x == 1) ? s->count_not_started-1 : s->count_not_started;
            s_next->count_finished = (s_next->x == W+1) ? s->count_finished+1 : s->count_finished;

            for (int z = 0; z < k; z++) {
                (s_next->ek)[z] = max(
                    (s_next->ek)[z], 
                    s_next->t + (ins->t)[s_next->x][(s_next->xk)[z]]
                );
			}
			for (int z = k+1; z < J-s_next->count_finished; z++) {
                (s_next->ek)[z] = max(
                    (s_next->ek)[z], 
                    s_next->t + (ins->t)[s_next->x][(s_next->xk)[z]]
                );
			}
            
            // capita solo nel caso euristico
            if (s_next->t > ins->U){
                state_destroy(allocator, s_next);
                continue;
            }

            bool pushed = pool_try_push(pool, allocator, s_next);
            if (pushed){
                s_next->pred = s;
                ref_count++;
                sr->push_success_count++;
            } else{
                state_destroy(allocator, s_next);
                sr->push_fail_count++;
            }
        }

        assert (number_of_extensions >= 1 && number_of_extensions <= ins->max_number_of_extensions);
        
        (sr->extensions_from_each_state_count[number_of_extensions])++;
    
        if (ref_count > 0){
            s->ref_count = ref_count;
        } else{
            State* cur = s;
            State* pred = NULL;
            do {
                pred = cur->pred;
                state_destroy(allocator, cur);

                // capita solo nel caso euristico 
                if (pred == NULL){
                    break;
                }
                //assert (pred != NULL);
                cur = pred;
                cur->ref_count--;
            } while (cur->ref_count == 0);
        }
    }

    if (best_state == NULL){
        //printf("ho tagliato tutti i path verso soluzioni migliori della trivial\n");
    }

    clock_t toc = clock();

    //print_optimal_solution(best_state);

    sr->z_optimal = best_obj_val;
    sr->execution_time = (double)(toc - tic) / CLOCKS_PER_SEC;

    sr->bitset_used_size = LIMIT;
    sr->state_allocations_count = allocator->count;

    // printf("solver_run number of extensions = ");
    // for (int i=0; i <= ins->max_number_of_extensions; i++){
    //     printf("%d ", count_number_of_extensions[i]);
    // }
    // printf("\n");
    // printf("solver_run z* = %d , %.2f %%\n", best_obj_val, -100.0 * ((float)ins->U - best_obj_val) / (ins->U));
    // printf("solver_run execution_time = %f s\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    // printf("solver_run LIMIT = %d\n", LIMIT);
    // printf("solver_run count = %d \n", allocator->count);

    // safe_free(count_number_of_extensions);
    // count_number_of_extensions = NULL;

    heuristic_extension_destroy(he);

    pool_free(pool, allocator);
    state_allocator_destroy(allocator);
}

