#ifndef SOLVER_H
#define SOLVER_H

#define POOL_SIZE 1000000
#define BITSET_ARR_LEN (POOL_SIZE+63)/64

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "utils.h"
#include "instance.h"

// --- public -----------------------------------------------------------------

typedef struct {
    
    int     z_optimal;
    float   execution_time;

    int     explored_states_count;
    int*    extensions_from_each_state_count;

    int     push_success_count; // 
    int     push_fail_count;
    int     bitset_used_size;
    int     state_allocations_count;

} SolverResult;

SolverResult*   solver_result_create    ();
void            solver_result_destroy   (SolverResult* sr);
void            solver_result_clear     (SolverResult* sr);
void            solver_result_print     (SolverResult* sr);

void    solver_run              (SolverResult* sr, Instance* ins);
void    solver_heuristic_run    (SolverResult* sr, Instance* ins, int extensions_threshold);

// --- private ----------------------------------------------------------------

// global variables
extern int LIMIT;
extern int J;
extern int W;

typedef struct State {
    
    int     t;
    int     x;
    int*    xk; // array of length J
    int*    ek; // array of length J
    int     count_not_started;
    int     count_finished;

    int             id;
    int             idx_in_time_slot;
    struct State*   pred;
    int             ref_count;

} State;

typedef struct {

    State*  memory_state_ptrs;
    int*    memory_xk_ek_ptrs;
    State*  offset;
    State** released_memory_state_ptrs;
    size_t  released_cur_size;

    int     count;

} StateAllocator;


State*  state_create                (StateAllocator* a);
void    state_print                 (State* s);
void    state_destroy               (StateAllocator* a, State* s);
bool    state_is_final              (State* s); 
bool    state_is_workstation_busy   (State* s, int i);
//bool    state_dominates             (State* s, State* other);


StateAllocator* state_allocator_create  ();
void            state_allocator_destroy (StateAllocator* a);
State*          state_allocator_get     (StateAllocator* a);
void            state_allocator_release (StateAllocator* a, State* s);

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

typedef uint64_t* BitSet;

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
void    pool_free       (Pool* pool, StateAllocator* a);
bool    pool_try_push   (Pool* pool, StateAllocator* a, State* s);
bool    pool_is_empty   (Pool* pool);
State*  pool_pop        (Pool* pool); // require: pool is not empty  

/*****************************************************************************/

typedef struct {
    int     extensions_max;
    int     extensions_threshold;
    int*    k;
    int*    k_times;
    int     k_cur_size;
    int*    best_k;
    int     best_k_cur_size;
} HeuristicExtension;

HeuristicExtension* heuristic_extension_create          (int extensions_max, int extensions_threshold);
void    heuristic_extension_destroy                     (HeuristicExtension* he);
void    heuristic_extension_clear                       (HeuristicExtension* he);
void    heuristic_extension_add                         (HeuristicExtension* he, int k, int time);
void    heuristic_extension_calculate_best_extensions   (HeuristicExtension* he);

#endif 