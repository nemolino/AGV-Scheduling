#include "solver.h"

#define SIZE 100000 // quanti stati allocati contemporaneamente al massimo?

StateAllocator* state_allocator_create()
{
    StateAllocator* a = (StateAllocator*)safe_malloc(sizeof(StateAllocator));
    a->memory_state_ptrs = (State*)safe_malloc(SIZE * sizeof(State));
    a->memory_xk_ek_ptrs = (int*)safe_calloc(SIZE * 2*J, sizeof(int));
    for (int i=0; i < SIZE; i++){
        (a->memory_state_ptrs)[i].xk = a->memory_xk_ek_ptrs + i*2*J;
        (a->memory_state_ptrs)[i].ek = a->memory_xk_ek_ptrs + i*2*J + J;
    }
    a->offset = a->memory_state_ptrs;
    a->released_memory_state_ptrs = (State**)safe_malloc(SIZE * sizeof(State*));
    for (int i=0; i < SIZE; i++){
        (a->released_memory_state_ptrs)[i] = NULL;
    }
    a->released_cur_size = 0;

    a->count = 0;
    return a;
}

void state_allocator_destroy(StateAllocator* a)
{
    safe_free(a->memory_state_ptrs);
    a->memory_state_ptrs = NULL;

    safe_free(a->memory_xk_ek_ptrs);
    a->memory_xk_ek_ptrs = NULL;

    safe_free(a->released_memory_state_ptrs);
    a->released_memory_state_ptrs = NULL;

    free(a);
    a = NULL;
}

State* state_allocator_get(StateAllocator* a)
{
    if (a->released_cur_size == 0){
        State* x = a->offset;
        a->offset++;
        a->count++;
        return x;
    } else {
        a->released_cur_size--;
        State* x = a->released_memory_state_ptrs[a->released_cur_size];
        a->released_memory_state_ptrs[a->released_cur_size] = NULL;
        return x;
    }
}

void state_allocator_release(StateAllocator* a, State* s)
{
    a->released_memory_state_ptrs[a->released_cur_size] = s;
    a->released_cur_size++;
    assert (a->released_cur_size <= SIZE);
}