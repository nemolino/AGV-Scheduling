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

bool state_is_xk_busy (State* s, int k)
{
    // TODO binary search to find k in arr[xk,ek)
    
    return true;
}

void state_set_t(State* s, int value)
{
    (s->arr)[T] = value;
}

void state_set_x(State* s, int value)
{
    (s->arr)[X] = value;
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