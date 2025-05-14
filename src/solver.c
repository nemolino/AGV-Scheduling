#include "solver.h"

int LIMIT;
int J;
int W;
    
void solver_run(Instance* ins)
{
    // global variables
    J = ins->J;
    W = ins->W;
    LIMIT = 0;
    
    StateAllocator *allocator = state_allocator_create();

    clock_t tic = clock();

    int best_obj_val = ins->U;
    State* best_state = NULL;

    Pool* pool = pool_create(ins->U);

    State* initial_state = state_create(allocator);
    initial_state->count_not_started = J;
    pool_try_push(pool, allocator, initial_state);

    while (!pool_is_empty(pool)){

        State* s = pool_pop(pool);

        // printf("popped state : ");
        // state_print(s);
        
        // il primo stato finale che raggiungo è la soluzione ottima, 
        // perchè estraggo gli stati dal pool in ordine di tempo crescente
        if (state_is_final(s)){
            assert (s->t <= best_obj_val);
            printf("SOLUZIONE OTTIMA TROVATA : stato finale raggiunto\n");
            best_obj_val = s->t;
            best_state = s;
            break;
        }
        
        int ref_count = 0;

        for (
            int k = J-1-(s->count_finished); 
            k >= max(0, s->count_not_started-1); 
            k--
        ) {
            
            // trying to move towards job k

            //	grazie a k := J - 1 - S.countFinished :
			//		non mi muovo mai verso il serbatoio finale, cioè ho sempre dest != M+1
			// 	grazie a k >= max(0, S.countNotStarted-1)
			// 		mi muovo verso il serbatoio iniziale al massimo 1 volta, cioè ho dest == 0 al massimo 1 volta

            int dest = (s->xk)[k];

            // mi muovo solo verso macchine con la dx libera o con a dx il serbatoio finale
			if (dest+1 != W+1 && state_is_workstation_busy(s, dest+1)) {

                // idea per un'euristica: rafforzare questa condizione.
                // se tagliamo destinazioni possibili, allora potenzialmente tagliamo il path verso l'ottimo
                // (a meno che in qualche modo non dimostriamo non siano ottimali).
                // comunque, se scegliamo sempre almeno una destinazione possibile, a una soluzione feasible si arriva per forza.
                // il gioco è cercare di tagliare destinazioni feasible in modo euristico.
                // se taglio solo destinazioni dominate, non perdo l'ottimalità.
                // se taglio anche destinazioni non dominate, potrei perdere l'ottimalità.
                
                // TODO trovare criteri di taglio sensati. 
                // es. vado solo verso le migliori k postazioni per minimo tempo di spostamento.
                // es. vado solo verso le migliori k postazioni per minimo tempo di attesa una volta arrivati.
                // es. vado solo verso le migliori k postazioni per minimo tempo di spostamento + attesa una volta arrivati.
                // valutarli in base a quanto peggiora l'ottimo in percentuale.

				continue;
			}

            State* s_next = state_create(allocator);//state_get_or_create(poolfree);

            // passa il tempo di spostamento
            s_next->t = s->t + (ins->t)[s->x][dest];
            
            // prima di muovere il job bisogna aspettare che la sua lavorazione finisca
            s_next->t = max(s_next->t, (s->ek)[k]);

            // muovo il job verso la macchina successiva
            // passa il tempo di spostamento
            s_next->t += (ins->t)[dest][dest+1];
            s_next->x = dest+1;

            for (int i = 0; i < J; i++) {
                (s_next->xk)[i] = (s->xk)[i];
                (s_next->ek)[i] = (s->ek)[i];
            }
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
            
            bool pushed = pool_try_push(pool, allocator, s_next);
            if (pushed){
                s_next->pred = s;
                ref_count++;
            } else{
                state_destroy(allocator, s_next);
            }
        }
    
        if (ref_count > 0){
            s->ref_count = ref_count;
        } else{
            State* cur = s;
            State* pred = NULL;
            do {
                pred = cur->pred;
                state_destroy(allocator, cur);
                assert (pred != NULL);
                cur = pred;
                cur->ref_count--;
            } while (cur->ref_count == 0);
        }
    }

    clock_t toc = clock();

    // print_optimal_solution(best_state);
    
    printf("solver_run execution_time = %f s\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("solver_run z* = %d\n", best_obj_val);
    printf("solver_run LIMIT = %d\n", LIMIT);
    printf("solver_run count = %d \n", allocator->count);

    // for (int i=0; i<=ins->U; i++){
    //     printf("%zu ", pool->time_slots[i]->size);
    // }
    // printf("\n");

    printf("free ...\n");
    pool_free(pool, allocator);
    state_allocator_destroy(allocator);
}

void print_optimal_solution(State* best_state)
{
    printf("print_optimal_solution ...\n");
    State* cur = best_state;
    do {
        state_print(cur);
        cur = cur->pred;
    } while(cur != NULL);
}