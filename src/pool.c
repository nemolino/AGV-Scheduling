#include "solver.h"

#define POOL_SIZE 100000

Pool* pool_create(int U)
{
    Pool* pool = (Pool*)safe_malloc(sizeof(Pool));
    pool->time_slots = (Slot**)safe_malloc((U+1) * sizeof(Slot*));
    for (int i=0; i < U+1; i++){
        pool->time_slots[i] = slot_create();
    }
    pool->time_slots_len = U+1;
    pool->cur_slot = 0;

    pool->ID_next = 0;
    pool->ID_to_state = (State**)safe_malloc(POOL_SIZE * sizeof(State*));
    for (int i=0; i < POOL_SIZE; i++){
        pool->ID_to_state[i] = NULL;
    }
    pool->free_states = (State**)safe_malloc(POOL_SIZE * sizeof(State*));
    for (int i=0; i < POOL_SIZE; i++){
        pool->free_states[i] = NULL;
    }
    pool->free_states_len = 0;

    return pool;
}

void pool_add_initial_state(Pool* pool, State* s)
{
    s->id = pool->ID_next;
    pool->ID_next++;
    slot_insert(pool->time_slots[state_get_t(s)], s);
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

// REQUIRES : pool_is_empty(pool) == false
State* pool_pop(Pool* pool)
{
    State* s = slot_delete(
        pool->time_slots[pool->cur_slot], 
        (pool->time_slots[pool->cur_slot])->tail 
    );

    // ... update dei bitset
    // ... id dello stato poppato e stato poppato stesso saranno messi nei free_states, ma forse non lo faremo qui
    return s;
}

void pool_free(Pool* pool)
{
    for (int i=0; i < pool->time_slots_len; i++){
        slot_free(pool->time_slots[i]);
    }
    free(pool->time_slots);

    for (int i=0; i < POOL_SIZE; i++){
        if (pool->ID_to_state[i]){
            state_destroy(pool->ID_to_state[i]);
        }
    }
    free(pool->ID_to_state);

    for (int i=0; i < pool->free_states_len; i++){
        state_destroy(pool->free_states[i]);
    }
    free(pool->free_states);

    free(pool);
}