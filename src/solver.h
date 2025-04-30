#ifndef SOLVER_H
#define SOLVER_H

#define POOL_SIZE 1000000
#define BITSET_ARR_LEN (POOL_SIZE+63)/64

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "utils.h"
#include "instance.h"

// --- public -----------------------------------------------------------------

void solver_run (Instance* ins);

// --- private ----------------------------------------------------------------

// errori di double free in agguato
extern int STATE_ALLOC;
extern int STATE_FREE;

extern int CREO;
extern int CACHO;

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
    int*            arr;      

    int             id;
    struct Node*    slot_node;
    struct State*   pred;
    int             ref_count;
} State;
   
/*****************************************************************************/

/**
 * Node of a Doubly Linked List of states, with pointers to predecessor and successor
 */
typedef struct Node {
    State*          s;
    struct Node*    prev;
    struct Node*    next;
} Node;

/**
 * Doubly Linked List of states, with pointers to head and tail
 */
typedef struct {
    Node*   head;
    Node*   tail;
    int     size;
} DLL;

DLL*    dll_create  ();
void    dll_free    (DLL* dll);
Node*   dll_insert  (DLL* dll, State* s);
State*  dll_delete  (DLL* dll, Node* node);

/*****************************************************************************/

/**
 * A stack of free states, implemented with an array and having max size = POOL_SIZE
 */
typedef struct {
    State** free_states; 
    int     free_states_size;
} PoolFree;


PoolFree*   poolfree_create         ();
void        poolfree_free           (PoolFree* pf);
bool        poolfree_any            (PoolFree* pf);
void        poolfree_push           (PoolFree* pf, State* s);
void        poolfree_push_iterative (PoolFree* pf, State* s);
State*      poolfree_pop            (PoolFree* pf);

/*****************************************************************************/

/**
 * A stack of IDs, implemented with an array and having max size = POOL_SIZE
 */
typedef struct {
    int*    stack; 
    int     stack_top;
    State** id_to_state; 
} IDs;

IDs*    ids_create  ();
void    ids_free    (IDs* ids);
void    ids_assign  (IDs* ids, State* s);
void    ids_release (IDs* ids, State* s);


/*****************************************************************************/

// bitset

// J = W+2 = 16
// POOL_SIZE = 100_000
// we need 3 * 16 * 16 = 768 bitsets, 
// each bitset is composed of 1563 int64 = 12504 bytes -> 9.603072 MB

// POOL_SIZE = 1_000_000
// each bitset is composed of 15625 int64 = 125000 bytes

// we need 3 * 17 * 17 =  867 bitsets -> 108.375 MB
// we need 3 * 18 * 18 =  972 bitsets -> 121.5   MB
// we need 3 * 19 * 19 = 1083 bitsets -> 135.375 MB
// we need 3 * 20 * 20 = 1200 bitsets -> 150.000 MB

// con i bitset il collo di bottiglia non è lo spazio di memoria,
// posso anche farli di dimensione fissa

// typedef struct {
//     uint64_t*   arr;
// } BitSet;

typedef u_int64_t* BitSet;

// per rendere le operazioni più veloci sfrutto il fatto che conosco l'id più grande mai dato
// -> nessun bitset avrà mai settati dei bit > id 

BitSet  bitset_create               (); // can contain up to 64 * BITSET_ARR_LEN entries 
void    bitset_destroy              (BitSet bs);
void    bitset_set_bit              (BitSet bs, int x);
void    bitset_clear_bit            (BitSet bs, int x);
void    bitset_clear_all            (BitSet bs);
void    bitset_copy                 (BitSet dest, BitSet src);
void    bitset_inplace_intersection (BitSet bs, BitSet other);
void    bitset_inplace_difference   (BitSet bs, BitSet other);
bool    bitset_any                  (BitSet bs);
bool    bitset_none                 (BitSet bs);

//int     bitset_next_set             (BitSet bs, int start_idx); 

/*****************************************************************************/

/**
 * ... TODO
 */
typedef struct {

    DLL**   time_slots;
    int     time_slots_len;     // equal to U+1
    int     cur_slot;

    IDs*    ids;

} Pool;

Pool*   pool_create     (int U);

void    pool_free       (Pool* pool);

// action:      se s è non dominato, aggiunge s al pool degli stati e ritorna true
//              se s è dominato, ritorna false
bool    pool_try_push   (Pool* pool, PoolFree* pf, State* s);

bool    pool_is_empty   (Pool* pool);

// require: pool is not empty  
State*  pool_pop        (Pool* pool);


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
State*  state_get_or_create         (PoolFree* pf);
void    state_clear_all             (State* s);
void    state_print                 (State* s);
void    state_destroy               (State* s);
bool    state_is_final              (State* s); 
bool    state_is_workstation_busy   (State* s, int i);
bool    state_dominates             (State* s, State* other);

//      setters
void    state_set_t                 (State* s, int value);
void    state_set_x                 (State* s, int value);
void    state_set_xk                (State* s, int k, int value);
void    state_set_xk_all_by_copy    (State* s, State* other);
void    state_set_ek                (State* s, int k, int value);
void    state_set_ek_all_by_copy    (State* s, State* other);
void    state_set_count_not_started (State* s, int value);
void    state_set_count_finished    (State* s, int value);
void    state_set_id                (State* s, int value);
void    state_set_pred              (State* s, State* other);
void    state_set_ref_count         (State* s, int value);

//      getters
int     state_get_t                 (State* s);
int     state_get_x                 (State* s);
int     state_get_xk                (State* s, int k);
int     state_get_ek                (State* s, int k);
int     state_get_count_not_started (State* s);
int     state_get_count_finished    (State* s);

#endif 