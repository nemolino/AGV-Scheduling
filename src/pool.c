#include "solver.h"

Pool* pool_create(int U)
{
    Pool* pool = (Pool*)safe_malloc(sizeof(Pool));

    pool->time_slots = (Vector**)safe_malloc((U+1) * sizeof(Vector*));
    for (int i = 0; i < U+1; i++){
        pool->time_slots[i] = vector_create();
    }
    pool->time_slots_len = U+1;
    pool->cur_slot = 0;
    pool->cur_slot_index = 0;

    pool->ids = ids_create();
    pool->id_container_aligned = (int*)safe_calloc(POOL_SIZE, sizeof(int));

    // creazione dei bitset

    pool->on = (BitSet**)safe_malloc(J * sizeof(BitSet*));
    pool->on[0] = (BitSet*)safe_malloc(J * (W+2) * sizeof(BitSet));
    pool->geq = (BitSet**)safe_malloc(J * sizeof(BitSet*));
    pool->geq[0] = (BitSet*)safe_malloc(J * (W+2) * sizeof(BitSet));
    pool->leq = (BitSet**)safe_malloc(J * sizeof(BitSet*));
    pool->leq[0] = (BitSet*)safe_malloc(J * (W+2) * sizeof(BitSet));
    for(int k = 1; k < J; k++) {
        pool->on[k] = pool->on[0] + k * (W+2);
        pool->geq[k] = pool->geq[0] + k * (W+2);
        pool->leq[k] = pool->leq[0] + k * (W+2);
    }
    for (int k = 0; k < J; k++) {
        for (int i = 0; i <= W+1; i++) {
            (pool->on)[k][i] = bitset_create();
            (pool->geq)[k][i] = bitset_create();
            (pool->leq)[k][i] = bitset_create();
        }
    }
    pool->candidate_aligned = bitset_create();
    pool->candidate_dominators_non_aligned = bitset_create();
    pool->candidate_dominated_non_aligned = bitset_create();
    
    return pool;
}

void pool_free(Pool* pool)
{
    for (int i=0; i < pool->time_slots_len; i++){
        vector_destroy(pool->time_slots[i]);
    }
    free(pool->time_slots);

    ids_free(pool->ids);
    free(pool->id_container_aligned);

    // bitsets

    for (int k = 0; k < J; k++) {
        for (int i = 0; i <= W+1; i++) {
            bitset_destroy((pool->on)[k][i]);
            bitset_destroy((pool->geq)[k][i]);
            bitset_destroy((pool->leq)[k][i]);
        }
    }
    free(pool->on[0]);
    free(pool->on);
    free(pool->geq[0]);
    free(pool->geq);
    free(pool->leq[0]);
    free(pool->leq);
    bitset_destroy(pool->candidate_aligned);
    bitset_destroy(pool->candidate_dominators_non_aligned);
    bitset_destroy(pool->candidate_dominated_non_aligned);

    free(pool);
}

bool dominates_aligned(State* s, State* s2)
{
    if (s->x == s2->x){
        return s->t <= s2->t;
    }

    // REQUIRES : i job sono perfettamente allineati
    int check_from = max(0, s->count_not_started-1);
    int check_to = J - s->count_finished;
    for (int k = check_from; k < check_to; k++) {
        if ((s->ek)[k] > (s2->ek)[k]) {
            return false;
        }
    }
    return true;
} 

bool dominates_non_aligned(State* s, State* s2)
{
    // REQUIRES : i job non sono perfettamente allineati
    if (s->t > s2->t) {
        return false;
    }
    int check_from = max(0, s->count_not_started-1);
    int check_to = J - s->count_finished;
    for (int k = check_from; k < check_to; k++) {
        if (((s->xk)[k] == (s2->xk)[k]) && ((s->ek)[k] > (s2->ek)[k])) {
            return false;
        }
    }
    return true;
}

bool pool_try_push(Pool* pool, PoolFree* pf, State* s)
{
    assert (s != NULL);

    // --- checking if some state in the pool dominates s ---------------------

	// --- exact alignment check (si possono diminuire le intersezioni credo)

    bitset_clear_all(pool->candidate_aligned);
    bitset_copy(
        pool->candidate_aligned, 
        (pool->on)[0][(s->xk)[0]]
    );
    for (int k = 1; k < J; k++) {
        bitset_inplace_intersection(
            pool->candidate_aligned, 
            (pool->on)[k][(s->xk)[k]]
        );
    }

    int id_container_aligned_len = 0; 

    if (bitset_any(pool->candidate_aligned)){
        // itero sui bit a 1 di pool->candidate_aligned
        int offset = 0;
        //for (size_t i = 0; i < BITSET_ARR_LEN; i++) {
        for (int i = 0; i < LIMIT; i++) {
            uint64_t word = pool->candidate_aligned[i];
            while (word != 0){
                uint64_t t = word & -word;
                int id = offset + __builtin_ctzll(word);
                pool->id_container_aligned[id_container_aligned_len] = id;
                id_container_aligned_len++;
                State* dominator = ids_get_state_from_id(pool->ids, id);
                if (dominates_aligned(dominator,s)){
                    return false;
                }
                word ^= t;
            }
            offset += 64;
        }
    }
    
    // --- non-exact alignment check

    bitset_clear_all(pool->candidate_dominators_non_aligned);

    int k;
    if (s->count_finished > 0){
        bitset_copy(
            pool->candidate_dominators_non_aligned, 
            (pool->on)[J - s->count_finished][(s->xk)[J-s->count_finished]]
        );
        k = J- s->count_finished-1;
    }else{
        assert (s->count_finished == 0);
        bitset_copy(
            pool->candidate_dominators_non_aligned, 
            (pool->geq)[J-1][(s->xk)[J-1]]
        );
        k = J-2;
    }

    for (; k >= s->count_not_started; k--) {
        bitset_inplace_intersection(
            pool->candidate_dominators_non_aligned, 
            (pool->geq)[k][(s->xk)[k]]
        );
    }

    bitset_inplace_difference(
        pool->candidate_dominators_non_aligned, 
        pool->candidate_aligned
    );

    if (bitset_any(pool->candidate_dominators_non_aligned)){
        // itero sui bit a 1 di pool->candidate_dominators_non_aligned
        int offset = 0;
        //for (size_t i = 0; i < BITSET_ARR_LEN; i++) {
        for (int i = 0; i < LIMIT; i++) {
            uint64_t word = pool->candidate_dominators_non_aligned[i];
            while (word != 0){
                uint64_t t = word & -word;
                int id = offset + __builtin_ctzll(word);
                // pool->id_container_aligned[id_container_aligned_len] = id;
                // id_container_aligned_len++;
                State* dominator = ids_get_state_from_id(pool->ids, id);
                if (dominates_non_aligned(dominator,s)){
                    return false;
                }
                word ^= t;
            }
            offset += 64;
        }
    }
    
    // --- checking if s dominates some state in the pool ---------------------
    
    // --- exact alignment check
    
    if (id_container_aligned_len > 0){
        for (int i=0; i < id_container_aligned_len; i++){
            int id = pool->id_container_aligned[i];
            State* dominated = ids_get_state_from_id(pool->ids, id);
            if (dominates_aligned(s,dominated)){

                vector_delete_at_index(
                    pool->time_slots[dominated->t], 
                    dominated->idx_in_time_slot
                );
                
                for (int k = 0; k < J; k++) {
                    int k_cur_station = (dominated->xk)[k];
                    bitset_clear_bit((pool->on)[k][k_cur_station], id);
                    for (int i = 0; i <= k_cur_station; i++) {
                        bitset_clear_bit((pool->geq)[k][k_cur_station], id);
                    }
                    for (int i = k_cur_station; i <= W+1; i++) {
                        bitset_clear_bit((pool->leq)[k][k_cur_station], id);
                    }
                }

                ids_release(pool->ids, dominated);
                poolfree_push_iterative(pf, dominated);
            }
        }
    }
    
    // --- non-exact alignment check

    bitset_clear_all(pool->candidate_dominated_non_aligned);

    k = 0;
    if (s->count_not_started > 0){
        bitset_copy(
            pool->candidate_dominated_non_aligned, 
            (pool->on)[s->count_not_started-1][(s->xk)[s->count_not_started-1]]
        );
        k = s->count_not_started;
    }else{
        assert (s->count_not_started == 0);
        bitset_copy(
            pool->candidate_dominated_non_aligned, 
            (pool->leq)[0][(s->xk)[0]]
        );
        k = 1;
    }

    for (; k <= J-s->count_finished-1; k++) {
        bitset_inplace_intersection(
            pool->candidate_dominated_non_aligned, 
            (pool->leq)[k][(s->xk)[k]]
        );
    }

    bitset_inplace_difference(
        pool->candidate_dominated_non_aligned, 
        pool->candidate_aligned
    );

    if (bitset_any(pool->candidate_dominated_non_aligned)){
        // itero sui bit a 1 di pool->candidate_dominated_non_aligned
        int offset = 0;
        //for (size_t i = 0; i < BITSET_ARR_LEN; i++) {
        for (int i = 0; i < LIMIT; i++) {
            uint64_t word = pool->candidate_dominators_non_aligned[i];
            while (word != 0){
                uint64_t t = word & -word;
                int id = offset + __builtin_ctzll(word);
                State* dominated = ids_get_state_from_id(pool->ids, id);
                if (dominates_aligned(s,dominated)){

                    vector_delete_at_index(
                        pool->time_slots[dominated->t], 
                        dominated->idx_in_time_slot
                    );

                    
                    for (int k = 0; k < J; k++) {
                        int k_cur_station = (dominated->xk)[k];
                        bitset_clear_bit((pool->on)[k][k_cur_station], id);
                        for (int i = 0; i <= k_cur_station; i++) {
                            bitset_clear_bit((pool->geq)[k][k_cur_station], id);
                        }
                        for (int i = k_cur_station; i <= W+1; i++) {
                            bitset_clear_bit((pool->leq)[k][k_cur_station], id);
                        }
                    }

                    ids_release(pool->ids, dominated);
                    poolfree_push_iterative(pf, dominated);
                }
                word ^= t;
            }
            offset += 64;
        }
    }

    // --- inserisco lo stato -------------------------------------------------

    ids_assign(pool->ids, s);
    assert (s->id >= 0);

    Vector* vec = pool->time_slots[s->t];
    vector_append(vec, s);
    s->idx_in_time_slot = vec->size-1;

    for (int k = 0; k < J; k++) {
		int k_cur_station = (s->xk)[k];
        bitset_set_bit((pool->on)[k][k_cur_station], s->id);
		for (int i = 0; i <= k_cur_station; i++) {
            bitset_set_bit((pool->geq)[k][k_cur_station], s->id);
		}
        for (int i = k_cur_station; i <= W+1; i++) {
            bitset_set_bit((pool->leq)[k][k_cur_station], s->id);
		}
	}

    return true;
}


bool pool_is_empty(Pool* pool)
{
    while (pool->cur_slot < pool->time_slots_len){
        Vector* vec = pool->time_slots[pool->cur_slot];
        while (pool->cur_slot_index < vec->size){
            if (vec->data[pool->cur_slot_index] != NULL){
                return false;
            }
            pool->cur_slot_index++;
        }
        pool->cur_slot++;
        pool->cur_slot_index = 0;
    }
    return true;
}


State* pool_pop(Pool* pool)
{
    assert(!pool_is_empty(pool));

    assert (
        pool->cur_slot_index >= 0 && 
        pool->cur_slot_index < pool->time_slots[pool->cur_slot]->size
    );
    State* s = (pool->time_slots[pool->cur_slot])->data[pool->cur_slot_index];
    assert (s != NULL);
    (pool->time_slots[pool->cur_slot])->data[pool->cur_slot_index] = NULL;
    pool->cur_slot_index++;


    for (int k = 0; k < J; k++) {
		int k_cur_station = (s->xk)[k];
        bitset_clear_bit((pool->on)[k][k_cur_station], s->id);
		for (int i = 0; i <= k_cur_station; i++) {
            bitset_clear_bit((pool->geq)[k][k_cur_station], s->id);
		}
        for (int i = k_cur_station; i <= W+1; i++) {
            bitset_clear_bit((pool->leq)[k][k_cur_station], s->id);
		}
	}
    
    ids_release(pool->ids, s);

    return s;
}