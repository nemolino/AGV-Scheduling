#include "solver.h"


State* state_create(StateAllocator* a)
{
    State* s = state_allocator_get(a); //(State*)safe_malloc(sizeof(State));

    s->t = 0;
    s->x = 0;

    // s->xk = (int *)safe_calloc(2*J, sizeof(int));
    // s->ek = s->xk + J;
    for (int k=0; k < J; k++){
        s->xk[k] = 0;
        s->ek[k] = 0;
    }

    s->count_not_started = 0;
    s->count_finished = 0;

    s->id = -1;
    s->idx_in_time_slot = -1;
    s->pred = NULL;
    s->ref_count = 0;
    
    return s;
}

/*
State* state_get_or_create(PoolFree* pf)
{
    if (poolfree_any(pf)){
        CACHO++;
        return poolfree_pop(pf);
    } else{
        CREO++;
        return state_create();
    };
}

void state_clear_all (State* s)
{
    assert (s != NULL);

    s->t = 0;
    s->x = 0;
    for (int i = 0; i < J; i++) {
        (s->xk)[i] = 0;
        (s->ek)[i] = 0;
    }
    s->count_not_started = 0;
    s->count_finished = 0;

    s->id = -1;
    s->idx_in_time_slot = -1;
    s->pred = NULL;
    s->ref_count = 0;
}
*/

void state_print(State* s)
{
    printf("arr : t = %d , x = %d , xk = [ ", s->t, s->x);
    for (int i = 0; i < J; i++) {
        printf("%d ", (s->xk)[i]);
    }
    printf("] , ek = [ ");
    for (int i = 0; i < J; i++) {
        printf("%d ", (s->ek)[i]);
    }
    printf("] , not_started = %d , finished = %d", s->count_not_started, s->count_finished);
    printf(" |  pred : %p\n", s->pred);
}

void state_destroy(StateAllocator* a, State* s)
{   
    state_allocator_release(a,s);
    /*
    safe_free(s->xk);
    s->xk = NULL;
    safe_free(s);
    s = NULL;
    STATE_FREE++;
    */
}

bool state_is_final (State* s)
{
    return s->count_finished == J;
}

bool state_is_workstation_busy (State* s, int i)
{
    // could be a binary search
    for (int k = 0; k < J; k++) {
        if (s->xk[k] == i){
            return true;
        }
    }
    return false;
}

bool state_dominates (State* s, State* other)
{
    bool time_condition = (s->t > other->t);

	int check_to = J - s->count_finished;
	for (int k = 0; k < check_to; k++) {

        if ((s->xk)[k] < (other->xk)[k]) 
            return false;
		
        bool cond1 = (s->xk)[k] > (other->xk)[k] && time_condition;
        bool cond2 = (s->xk)[k] == (other->xk)[k] && (s->ek)[k] > (other->ek)[k];
		if (cond1 || cond2)
			return false;
	}
	return true;
}