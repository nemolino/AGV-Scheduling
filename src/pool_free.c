#include "solver.h"

/**
 * @brief allocates an empty PoolFree
 * 
 * @return PoolFree* 
 */
PoolFree* poolfree_create()
{
    PoolFree* pf = (PoolFree*)safe_malloc(sizeof(PoolFree));

    pf->free_states = (State**)safe_malloc(POOL_SIZE * sizeof(State*));
    for (int i=0; i < POOL_SIZE; i++){
        pf->free_states[i] = NULL;
    }
    pf->free_states_size = 0;

    return pf;
}

/**
 * @brief de-allocates pf and all the states that it contains
 * 
 * @param pf 
 */
void poolfree_free(PoolFree* pf)
{
    for (int i=0; i < pf->free_states_size; i++){
        state_destroy(pf->free_states[i]);
    }
    free(pf->free_states);
    free(pf);
}

/**
 * @brief returns true if pf is not empty, false otherwise
 * 
 * @param pf 
 * @return true if pf is not empty
 * @return false otherwise
 */
bool poolfree_any(PoolFree* pf)
{
    return pf->free_states_size > 0;
}

/**
 * @brief if pf is not full, adds s to pf. if pf is full, prints a message
 * 
 * @param pf 
 * @param s 
 */
void poolfree_push(PoolFree* pf, State* s)
{
    assert (s != NULL);
    assert (s->ref_count == 0);

    if (pf->free_states_size == POOL_SIZE){
        printf("PoolFree is full\n");
        STATE_FREE++;
        state_destroy(s);
    } else {
        (pf->free_states)[pf->free_states_size] = s;
        pf->free_states_size++;
    }
}

void poolfree_push_iterative(PoolFree* pf, State* s)
{
    State* cur = s;
    do {
        poolfree_push(pf,cur);
        cur = cur->pred;
        if (cur == NULL)
            return;
        assert (cur != NULL);
        cur->ref_count--;
    } while (cur->ref_count == 0);
}

/**
 * @brief extract a state from pf and returns it ready be used
 * 
 * @pre pf is not empty 
 * @param pf 
 * @return State*
 */
State* poolfree_pop(PoolFree* pf)
{
    assert (poolfree_any(pf));

    State* s = (pf->free_states)[pf->free_states_size-1];
    state_clear_all(s);
    (pf->free_states)[pf->free_states_size-1] = NULL;
    pf->free_states_size--;
    return s;
}