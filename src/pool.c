#include "solver.h"

Pool* pool_create(int U)
{
    Pool* pool = (Pool*)safe_malloc(sizeof(Pool));
    pool->time_slots = (DLL**)safe_malloc((U+1) * sizeof(DLL*));
    for (int i=0; i < U+1; i++){
        pool->time_slots[i] = dll_create();
    }
    pool->time_slots_len = U+1;
    pool->cur_slot = 0;

    pool->ids = ids_create();

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
    
    return pool;
}

void pool_free(Pool* pool)
{
    for (int i=0; i < pool->time_slots_len; i++){
        dll_free(pool->time_slots[i]);
    }
    free(pool->time_slots);

    ids_free(pool->ids);

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

    free(pool);
}

bool pool_try_push(Pool* pool, PoolFree* pf, State* s)
{
    assert (s != NULL);

    // vedere se lo stato è dominato da uno stato con tempo <= a lui
    
    for (int i=(s->arr)[T]; i >= pool->cur_slot; i--){
        DLL* dll = (pool->time_slots)[i];
        if (dll->size > 0){
            Node* cur = dll->head;
            do {
                if (state_dominates(cur->s, s)){
                    // printf("domination\n");
                    // state_print(cur->s);
                    // state_print(s);
                    // printf("\n");
                    
                    return false;
                }
                cur = cur->next;
            } while (cur);
        }
    }
    
    // vedere se lo stato domina qualche stato con tempo >= a lui
    for (int i=(s->arr)[T]; i < pool->time_slots_len; i++){
        DLL* dll = (pool->time_slots)[i];
        if (dll->size > 0){
            Node* cur = dll->head;
            Node* next = NULL;
            do {
                next = cur->next;
                if (state_dominates(s,cur->s)){
                    // printf("elimination\n");
                    // state_print(s);
                    // state_print(cur->s);
                    // printf("\n");

                    State* tmp = dll_delete(dll, (cur->s)->slot_node);
                    ids_release(pool->ids, tmp);
                    poolfree_push_iterative(pf, tmp);
                }
                cur = next;
            } while (cur);
        }
    }

    // inserisco lo stato

    ids_assign(pool->ids, s);
    assert (s->id >= 0);

    s->slot_node = dll_insert(pool->time_slots[state_get_t(s)], s);
    
    for (int k = 0; k < J; k++) {
		int k_cur_station = state_get_xk(s,k);
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
        if (((pool->time_slots)[pool->cur_slot])->size > 0){
            return false;
        }
        pool->cur_slot++;
    }
    return true;
}


State* pool_pop(Pool* pool)
{
    assert(!pool_is_empty(pool));

    State* s = dll_delete(
        pool->time_slots[pool->cur_slot], 
        (pool->time_slots[pool->cur_slot])->tail 
    );
    assert (s != NULL);

    for (int k = 0; k < J; k++) {
		int k_cur_station = state_get_xk(s,k);
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