#ifndef SOLVER_H
#define SOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "utils.h"
#include "instance.h"

struct Node;

typedef struct State {
    /*
        arr[0]                      = t
        arr[1]                      = x
        arr[2] ... arr[2+J-1]       = xk
        arr[2+J] ... arr[2+J+J-1]   = ek
        arr[2+J+J]                  = # not started
        arr[2+J+J+1]                = # finished
    */ 
    int* arr;      

    int id;
    struct Node* slot_node;
    struct State* pred;
} State;


typedef struct Node {
    State*          s;      // data
    struct Node*    prev;   // pointer to predecessor in Slot
    struct Node*    next;   // pointer to successor in Slot
} Node;


typedef struct Slot {
    Node*   head;
    Node*   tail;
    int     size;
} Slot;


typedef struct {

    Slot**  time_slots;
    int     time_slots_len;     // equal to U+1
    int     cur_slot;

    int     ID_next;
    State** ID_to_state; 
    State** free_states; 
    int     free_states_len;

    // altri campi
} Pool;


// --- State ------------------------------------------------------------------

extern int J;

extern int STATE_SIZE;
extern int T;
extern int X;
extern int XK;
extern int EK;
extern int COUNT_NOT_STARTED;
extern int COUNT_FINISHED;

void    init_globals                (Instance* ins);
State*  state_create                ();
State*  state_get_or_create         (Pool* pool);
void    state_clean_all_except_id   (State* s);
void    state_print                 (State* s);
void    state_destroy               (State* s);
bool    state_is_final              (State* s); 
bool    state_is_workstation_busy   (State* s, int i);

void    state_set_t                 (State* s, int value);
void    state_set_x                 (State* s, int value);
void    state_set_xk                (State* s, int k, int value);
void    state_set_xk_all_by_copy    (State* s, State* other);
void    state_set_ek                (State* s, int k, int value);
void    state_set_ek_all_by_copy    (State* s, State* other);
void    state_set_count_not_started (State* s, int value);
void    state_set_count_finished    (State* s, int value);
void    state_set_id                (State* s, int value);

int     state_get_t                 (State* s);
int     state_get_x                 (State* s);
int     state_get_xk                (State* s, int k);
int     state_get_ek                (State* s, int k);
int     state_get_count_not_started (State* s);
int     state_get_count_finished    (State* s);

// --- Slot -------------------------------------------------------------------

Slot*   slot_create                 ();
void    slot_insert                 (Slot* list, State* s);
State*  slot_delete                 (Slot* list, Node* node);
void    slot_print                  (Slot* list);
void    slot_free                   (Slot* list);

// --- Pool -------------------------------------------------------------------

Pool*   pool_create                 (int U);
void    pool_push_free_state        (Pool* pool, State* s);
bool    pool_any_free_state         (Pool* pool);
State*  pool_pop_free_state         (Pool* pool);
void    pool_add_state              (Pool* pool, State* s);
bool    pool_is_empty               (Pool* pool);
State*  pool_pop                    (Pool* pool);
void    pool_free                   (Pool* pool);

// --- Solver -----------------------------------------------------------------

void    solver_run                  (Instance* ins);

#endif 