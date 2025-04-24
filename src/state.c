#include "solver.h"

int J;

int STATE_SIZE;
int T;
int X;
int XK;
int EK;
int COUNT_NOT_STARTED;
int COUNT_FINISHED;

void init_globals(Instance* ins)
{
    J = ins->J;

    STATE_SIZE = 2 +J +J +2;
    T = 0;
    X = 1;
    XK = 2;
    EK = 2 +J;
    COUNT_NOT_STARTED = 2 +J +J;
    COUNT_FINISHED = 2 +J +J + 1;
}

State* state_create()
{
    State* s = (State*)safe_malloc(sizeof(State));
    s->arr = (int *)safe_calloc(STATE_SIZE, sizeof(int));
    s->id = -1;
    s->slot_node = NULL;
    s->pred = NULL;
    return s;
}

State* state_get_or_create(Pool* pool)
{
    State* s;
    if (pool_any_free_state(pool)){
        s = pool_pop_free_state(pool);
        state_clean_all_except_id(s);
    } else {
        s = state_create();
        state_set_id(s, pool->ID_next);
        pool->ID_next++;
    }
    return s;
}

void state_clean_all_except_id (State* s)
{
    for (int i = 0; i < STATE_SIZE; i++) {
        (s->arr)[i] = 0;
    }
    s->slot_node = NULL;
    s->pred = NULL;
}

void state_print(State* s)
{
    printf("arr : t = %d , x = %d , xk = [ ", (s->arr)[T], (s->arr)[X]);
    for (int i = XK; i < EK; i++) {
        printf("%d ", (s->arr)[i]);
    }
    printf("] , ek = [ ");
    for (int i = EK; i < COUNT_NOT_STARTED; i++) {
        printf("%d ", (s->arr)[i]);
    }
    printf("] , not_started = %d , finished = %d", (s->arr)[COUNT_NOT_STARTED], (s->arr)[COUNT_FINISHED]);
    printf(" |  pred : %p\n", s->pred);
}

void state_destroy(State* s)
{   
    free(s->arr);
    free(s);
}

bool state_is_final (State* s)
{
    return (s->arr)[COUNT_FINISHED] == J;
}

bool state_is_workstation_busy (State* s, int i)
{
    // find i in arr[xk,ek), could be a binary search
    for (int k = XK; k < EK; k++) {
       if ((s->arr)[k] == i){
        return true;
       }
    }
    return false;
}

void state_set_t(State* s, int value)
{
    (s->arr)[T] = value;
}

void state_set_x(State* s, int value)
{
    (s->arr)[X] = value;
}

void state_set_xk(State* s, int k, int value)
{
    (s->arr)[XK+k] = value;
}

void state_set_xk_all_by_copy(State* s, State* other)
{
    for (int i = XK; i < EK; i++) {
        (s->arr)[i] = (other->arr)[i];
    }
}

void state_set_ek(State* s, int k, int value)
{
    (s->arr)[EK+k] = value;
}

void state_set_ek_all_by_copy(State* s, State* other)
{
    for (int i = EK; i < COUNT_NOT_STARTED; i++) {
        (s->arr)[i] = (other->arr)[i];
    }
}

void state_set_count_not_started(State* s, int value)
{
    (s->arr)[COUNT_NOT_STARTED] = value;
}

void state_set_count_finished(State* s, int value)
{
    (s->arr)[COUNT_FINISHED] = value;
}

void state_set_id(State* s, int value)
{
    s->id = value;
}

int state_get_t(State* s)
{
    return (s->arr)[T];
}

int state_get_x(State* s)
{
    return (s->arr)[X];
}

int state_get_xk(State* s, int k)
{
    return (s->arr)[XK+k];
}

int state_get_ek(State* s, int k)
{
    return (s->arr)[EK+k];
}

int state_get_count_not_started(State* s)
{
    return (s->arr)[COUNT_NOT_STARTED];
}

int state_get_count_finished(State* s)
{
    return (s->arr)[COUNT_FINISHED];
}