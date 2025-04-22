#ifndef STATE_H
#define STATE_H

#include "instance.h"

#include <stdbool.h>

extern int J;

extern int STATE_SIZE;
extern int T;
extern int X;
extern int XK;
extern int EK;
extern int COUNT_NOT_STARTED;
extern int COUNT_FINISHED;

typedef struct State {
    /*
        arr[0]                      = t
        arr[1]                      = x
        arr[2] ... arr[2+J-1]       = xk
        arr[2+J] ... arr[2+J+J-1]   = ek
        arr[2+J+J]                  = # not started
        arr[2+J+J+1]                = # finished
        ... altre componenti da vedere
    */ 
    int* arr;          
    struct State *pred;
} State;

void    init_globals    (Instance* ins);
State*  state_create    ();
void    state_print     (State* s);
void    state_destroy   (State* s);
bool    state_is_final  (State* s);

// setters
void state_set_t                    (State* s, int value);
void state_set_x                    (State* s, int value);
void state_set_count_not_started    (State* s, int value);
void state_set_count_finished       (State* s, int value);



#endif 