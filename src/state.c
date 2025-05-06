#include "solver.h"

int STATE_ALLOC;
int STATE_FREE;
int CREO;
int CACHO;
int SAME_X;
int NOT_SAME_X;
int LIMIT;
int J;
int W;

void init_globals(Instance* ins)
{
    STATE_ALLOC = 0;
    STATE_FREE = 0;
    CREO = 0;
    CACHO = 0;
    SAME_X = 0;
    NOT_SAME_X = 0;
    LIMIT = 0;
    J = ins->J;
    W = ins->W;
}

State* state_create()
{
    State* s = (State*)safe_malloc(sizeof(State));

    s->t = 0;
    s->x = 0;
    s->xk = (int *)safe_calloc(2*J, sizeof(int));
    s->ek = s->xk + J;
    s->count_not_started = 0;
    s->count_finished = 0;

    s->id = -1;
    s->idx_in_time_slot = -1;
    s->pred = NULL;
    s->ref_count = 0;

    STATE_ALLOC++;
    return s;
}

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

void state_destroy(State* s)
{   
    safe_free(s->xk);
    s->xk = NULL;
    safe_free(s);
    s = NULL;
    STATE_FREE++;
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

// bool state_dominates (State* s, State* other)
// {
//     /*
//     se i due stati sono allineati e la posizione dell'AGV è uguale allora s domina se ha tempo minore
//     */
//     if ((s->arr)[T] > (other->arr)[T]) {
// 		return false;
// 	}
// 	//int check_from = max(0, (s->arr)[COUNT_NOT_STARTED]-1);
// 	int check_to = J - (s->arr)[COUNT_FINISHED];
// 	for (int k = 0; k < check_to; k++) {
//         if (
//             (s->arr)[XK+k] < (other->arr)[XK+k]
//         ) {
// 			return false;
// 		}
// 		if (
//             (s->arr)[XK+k] == (other->arr)[XK+k] && 
//             (s->arr)[EK+k] > (other->arr)[EK+k]
//         ) {
// 			return false;
// 		}
// 	}
// 	return true;
// }