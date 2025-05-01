#include "solver.h"

/**
 * @brief allocates an empty IDs
 * 
 * @return IDs* 
 */
IDs* ids_create()
{
    IDs* ids = (IDs*)safe_malloc(sizeof(IDs));
    
    ids->id_to_state = (State**)safe_malloc(POOL_SIZE * sizeof(State*));
    for (int i=0; i < POOL_SIZE; i++){
        ids->id_to_state[i] = NULL;
    }

    ids->stack = (int*)safe_malloc(POOL_SIZE * sizeof(int));
    for (int i=0, j=POOL_SIZE-1; i < POOL_SIZE; i++, j--){
        (ids->stack)[i] = j;
    }
    ids->stack_top = POOL_SIZE-1;

    return ids;
}

/**
 * @brief de-allocates ids and all the states that it contains
 * 
 * @param ids
 */
void ids_free(IDs* ids)
{
    // for (int i=0; i < POOL_SIZE; i++){
    //     if (ids->id_to_state[i]){
    //         state_destroy(ids->id_to_state[i]);
    //     }
    // }
    free(ids->id_to_state);
    free(ids->stack);
    free(ids);
}

/**
 * @brief assigns an available id to s
 * 
 * @param ids 
 * @param s 
 * @throw error if there are no available ids
 */
void ids_assign(IDs* ids, State* s)
{
    if (ids->stack_top < 0){
        printf("not enough IDs, POOL_SIZE must be increased\n");
        exit(EXIT_FAILURE);
    }
    s->id = ids->stack[ids->stack_top];
    ids->stack_top--;
    ids->id_to_state[s->id] = s;
}

/**
 * @brief releases the id of s
 * 
 * @param ids 
 * @param s 
 */
void ids_release(IDs* ids, State* s)
{
    ids->stack_top++;
    ids->stack[ids->stack_top] = s->id;
    ids->id_to_state[s->id] = NULL;
}