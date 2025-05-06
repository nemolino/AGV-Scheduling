#include "solver.h"

#define INITIAL_CAPACITY 128

Vector* vector_create()
{
    Vector* vec = (Vector*)safe_malloc(sizeof(Vector));
    vec->data = NULL;   //(State**)safe_malloc(capacity * sizeof(State*));
    vec->size = 0;
    vec->capacity = 0;
    return vec;
}

void vector_append(Vector* vec, State* s) 
{
    if (vec->size == 0) {
        assert (vec->capacity == 0 && vec->data == NULL);
        vec->data = (State**)safe_malloc(INITIAL_CAPACITY * sizeof(State*));
        vec->capacity = INITIAL_CAPACITY;
    } 
    else if (vec->size >= vec->capacity) {
        vec->capacity *= 2;
        vec->data = (State**)safe_realloc(vec->data, vec->capacity * sizeof(State*));
    }
    vec->data[vec->size] = s;
    vec->size++;
}

void vector_delete_at_index(Vector* vec, int idx)
{
    assert (idx >= 0 && idx < vec->size);
    vec->data[idx] = NULL;
}

void vector_destroy(Vector *vec) 
{
    if (vec->data != NULL){
        for (int i = 0; i < vec->size; i++){
            if (vec->data[i] != NULL){
                state_destroy(vec->data[i]);
            }
        }
        free(vec->data);
    }
    free(vec);
}