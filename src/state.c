#include "state.h"

#include <stdio.h>
#include <stdlib.h>

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
    State* s = (State*)malloc(sizeof(State));
    if (s == NULL) {
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    s->arr = (int *)calloc(STATE_SIZE, sizeof(int));
    if (s->arr == NULL) {
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

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
    (s->arr)[COUNT_FINISHED] == J;
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

