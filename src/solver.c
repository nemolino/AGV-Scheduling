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

    // clock_t tic2 = clock();
    // const int len = 20*20*3;
    // BitSet bss[len];
    // for (int i=0; i < len; i++){
    //     bss[i] = bitset_create();
    // } 
    // clock_t toc2 = clock();

    State* initial_state = state_get_or_create(poolfree);
    state_set_count_not_started(initial_state, J);
    pool_try_push(pool, poolfree, initial_state);

    while (!pool_is_empty(pool)){

        State* s = pool_pop(pool); 

        // printf("popped state : ");
        // state_print(s);

        // se lo stato ha tempo >= UB, la soluzione trivial è ottima
        // perchè estraggo gli stati dal pool in ordine di tempo crescente
        if (state_get_t(s) >= best_obj_val){
            printf("SOLUZIONE OTTIMA TROVATA : stato con tempo >= UB\n");
            poolfree_push_iterative(poolfree, s);
            break;
        }
        
        // il primo stato finale che raggiungo è la soluzione ottima, 
        // perchè estraggo gli stati dal pool in ordine di tempo crescente
        if (state_is_final(s)){
            assert (state_get_t(s) < best_obj_val);
            printf("SOLUZIONE OTTIMA TROVATA : stato finale raggiunto\n");
            best_obj_val = state_get_t(s);
            best_state = s;
            break;
        }
        
        int ref_count = 0;

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
			if (dest+1 != W+1 && state_is_workstation_busy(s, dest+1)) {
				continue;
			}

            State* s_next = state_get_or_create(poolfree);

            // passa il tempo di spostamento
            state_set_t(s_next, state_get_t(s) + t[state_get_x(s)][dest]);
            
            // prima di muovere il job bisogna aspettare che la sua lavorazione finisca
            state_set_t(s_next, max(state_get_t(s_next), state_get_ek(s,k)));

            // muovo il job verso la macchina successiva
            // passa il tempo di spostamento
            state_set_t(s_next, state_get_t(s_next) + t[dest][dest+1]);
            state_set_x(s_next, dest+1);

            state_set_xk_all_by_copy(s_next, s);
            state_set_xk(s_next, k, state_get_xk(s_next, k)+1);

            state_set_ek_all_by_copy(s_next, s);
            if (state_get_x(s_next) == W+1) {
                state_set_ek(s_next,k,-1);
			} else {
				state_set_ek(s_next,k,state_get_t(s_next)+p[state_get_x(s_next)]);
			}

            state_set_count_not_started(
                s_next, 
                (state_get_x(s_next) != 1) ? state_get_count_not_started(s) : state_get_count_not_started(s)-1
            );

            state_set_count_finished(
                s_next, 
                (state_get_x(s_next) != W+1) ? state_get_count_finished(s) : state_get_count_finished(s)+1
            );

            for (int z = 0; z < k; z++) {
                state_set_ek(
                    s_next,z,
                    max(
                        state_get_ek(s_next,z), 
                        state_get_t(s_next) + t[state_get_x(s_next)][state_get_xk(s_next,z)]
                    )
                );
			}
			for (int z = k+1; z < J-state_get_count_finished(s_next); z++) {
				state_set_ek(
                    s_next,z,
                    max(
                        state_get_ek(s_next,z), 
                        state_get_t(s_next) + t[state_get_x(s_next)][state_get_xk(s_next,z)]
                    )
                );
			}
            
            bool pushed = pool_try_push(pool, poolfree, s_next);
            if (pushed){
                state_set_pred(s_next, s);
                ref_count++;
            }else{
                // s_next è stato appena preso dai free_states o allocato, 
                // deve dunque avere ref_count == 0.
                // non lo facciamo in modo ricorsivo perchè il reference count di s non è ancora aggiornato
                poolfree_push(poolfree, s_next);
            }
        }
    
        if (ref_count > 0){
            state_set_ref_count(s, ref_count);
        } else{
            poolfree_push_iterative(poolfree, s);
        }
    }

    printf("fine LOOP...\n");

    printf("stampa sol ottima...\n");
    State* cur = best_state;
    do {
        state_print(cur);
        cur = cur->pred;
    } while(cur != NULL);

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
    // printf("bitsets creation execution_time = %f s\n", (double)(toc2 - tic2) / CLOCKS_PER_SEC);
    printf("solver_run z* = %d\n", best_obj_val);
    printf("solver_run state_alloc = %d , state_free = %d\n", STATE_ALLOC, STATE_FREE);
    printf("solver_run CREO = %d , CACHO = %d\n", CREO, CACHO);
}

