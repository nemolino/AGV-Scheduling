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

    Pool* pool = pool_create(U);
    PoolFree* poolfree = poolfree_create(); 

    State* initial_state = state_get_or_create(poolfree);
    initial_state->count_not_started = J;
    pool_try_push(pool, poolfree, initial_state);

    int count = 0;
    while (!pool_is_empty(pool)){

        State* s = pool_pop(pool); 
        count++;


        // printf("popped state : ");
        // state_print(s);

        // se lo stato ha tempo >= UB, la soluzione trivial è ottima
        // perchè estraggo gli stati dal pool in ordine di tempo crescente
        if (s->t >= best_obj_val){
            printf("SOLUZIONE OTTIMA TROVATA : stato con tempo >= UB\n");
            poolfree_push_iterative(poolfree, s);
            break;
        }
        
        // il primo stato finale che raggiungo è la soluzione ottima, 
        // perchè estraggo gli stati dal pool in ordine di tempo crescente
        if (state_is_final(s)){
            assert (s->t < best_obj_val);
            printf("SOLUZIONE OTTIMA TROVATA : stato finale raggiunto\n");
            best_obj_val = s->t;
            best_state = s;
            break;
        }
        
        int ref_count = 0;

        for (
            int k = J-1-(s->count_finished); 
            k >= max(0, (s->count_not_started)-1); 
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
				continue;
			}

            State* s_next = state_get_or_create(poolfree);

            // passa il tempo di spostamento
            s_next->t = s->t + t[s->x][dest];
            
            // prima di muovere il job bisogna aspettare che la sua lavorazione finisca
            s_next->t = max(s_next->t, (s->ek)[k]);

            // muovo il job verso la macchina successiva
            // passa il tempo di spostamento
            s_next->t = s_next->t + t[dest][dest+1];
            s_next->x = dest+1;

            for (int i = 0; i < J; i++) {
                (s_next->xk)[i] = (s->xk)[i];
                (s_next->ek)[i] = (s->ek)[i];
            }
            (s_next->xk)[k]++;
            if (s_next->x == W+1) {
                (s_next->ek)[k] = -1;
			} else {
                (s_next->ek)[k] = s_next->t + p[s_next->x];
			}

            s_next->count_not_started = (s_next->x != 1) ? s->count_not_started : s->count_not_started-1;
            s_next->count_finished = (s_next->x != W+1) ? s->count_finished : s->count_finished+1;

            for (int z = 0; z < k; z++) {
                (s_next->ek)[z] = max(
                    (s_next->ek)[z], 
                    s_next->t + t[s_next->x][(s_next->xk)[z]]
                );
			}
			for (int z = k+1; z < J-s_next->count_finished; z++) {
                (s_next->ek)[z] = max(
                    (s_next->ek)[z], 
                    s_next->t + t[s_next->x][(s_next->xk)[z]]
                );
			}
            
            bool pushed = pool_try_push(pool, poolfree, s_next);
            if (pushed){
                s_next->pred = s;
                ref_count++;
            }else{
                // s_next è stato appena preso dai free_states o allocato, 
                // deve dunque avere ref_count == 0.
                // non lo facciamo in modo ricorsivo perchè il reference count di s non è ancora aggiornato
                poolfree_push(poolfree, s_next);
            }
        }
    
        if (ref_count > 0){
            s->ref_count = ref_count;
        } else{
            poolfree_push_iterative(poolfree, s);
        }
    }

    printf("fine LOOP...\n");

    /*
    printf("stampa sol ottima...\n");
    State* cur = best_state;
    do {
        state_print(cur);
        cur = cur->pred;
    } while(cur != NULL);
    */

    printf("free ...\n");
    
    poolfree_push_iterative(poolfree, best_state);
    while (!pool_is_empty(pool)){
        State* s = pool_pop(pool); 
        poolfree_push_iterative(poolfree, s);
    }
    poolfree_free(poolfree);
    pool_free(pool);
    
    clock_t toc = clock();
    printf("... solver_run END\n");
    printf("solver_run execution_time = %f s\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    printf("solver_run z* = %d\n", best_obj_val);
    printf("solver_run state_alloc = %d , state_free = %d\n", STATE_ALLOC, STATE_FREE);
    printf("solver_run CREO = %d , CACHO = %d\n", CREO, CACHO);
    printf("solver_run LIMIT = %d\n", LIMIT);
    printf("solver_run count = %d\n", count);
}

