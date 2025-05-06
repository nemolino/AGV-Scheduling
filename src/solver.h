#ifndef SOLVER_H
#define SOLVER_H

#define POOL_SIZE 1000000
#define BITSET_ARR_LEN (POOL_SIZE+63)/64

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
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
extern int SAME_X;
extern int NOT_SAME_X;

extern int LIMIT;

typedef struct State {
    /*
        arr[0]                      = t
        arr[1]                      = x
        arr[2] ... arr[2+J-1]       = xk
        arr[2+J] ... arr[2+J+J-1]   = ek
        arr[2+J+J]                  = # not started
        arr[2+J+J+1]                = # finished
    */ 

    int     t;
    int     x;
    int*    xk;
    int*    ek;
    int     count_not_started;
    int     count_finished;

    int             id;
    int             idx_in_time_slot;
    struct State*   pred;
    int             ref_count;
} State;

/*****************************************************************************/

/**
 * Vector of states
 */
typedef struct {
    State** data;
    size_t  size;
    size_t  capacity;
} Vector;

Vector* vector_create           ();
void    vector_append           (Vector* vec, State* s);
void    vector_delete_at_index  (Vector* vec, int idx);
void    vector_destroy          (Vector *vec);

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

IDs*    ids_create              ();
void    ids_free                (IDs* ids);
void    ids_assign              (IDs* ids, State* s);
void    ids_release             (IDs* ids, State* s);
State*  ids_get_state_from_id   (IDs* ids, int id);


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

/*****************************************************************************/

/**
 * ... TODO
 */
typedef struct {

    Vector**    time_slots;
    int         time_slots_len;     // equal to U+1
    int         cur_slot;
    int         cur_slot_index;

    IDs*        ids;
    int*        id_container_aligned;

    BitSet**    on;
    BitSet**    geq;
    BitSet**    leq;
    BitSet      candidate_aligned;
    BitSet      candidate_dominators_non_aligned;;
    BitSet      candidate_dominated_non_aligned;

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
extern int W;

void    init_globals                (Instance* ins);
State*  state_create                ();
State*  state_get_or_create         (PoolFree* pf);
void    state_clear_all             (State* s);
void    state_print                 (State* s);
void    state_destroy               (State* s);
bool    state_is_final              (State* s); 
bool    state_is_workstation_busy   (State* s, int i);

#endif 