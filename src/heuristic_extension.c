#include "solver.h"

HeuristicExtension* heuristic_extension_create (int extensions_max, int extensions_threshold)
{
    HeuristicExtension* he = (HeuristicExtension*)safe_malloc(sizeof(HeuristicExtension));

    he->k = (int*)safe_calloc(extensions_max, sizeof(int));
    he->k_times = (int*)safe_calloc(extensions_max, sizeof(int));
    he->best_k = (int*)safe_calloc(extensions_threshold, sizeof(int));
    he->extensions_max = extensions_max;
    he->extensions_threshold = extensions_threshold;
    heuristic_extension_clear(he);
    return he;
}

void heuristic_extension_destroy (HeuristicExtension* he)
{
    safe_free(he->k);
    he->k = NULL;
    safe_free(he->k_times);
    he->k_times = NULL;
    safe_free(he->best_k);
    he->best_k = NULL;
    safe_free(he);
    he = NULL;
}

void heuristic_extension_clear (HeuristicExtension* he)
{
    he->k_cur_size = 0;
    for (int i=0; i < he->extensions_max; i++){
        he->k[i] = -1;
        he->k_times[i] = -1;
    }
    he->best_k_cur_size = 0;
    for (int i=0; i < he->extensions_threshold; i++){
        he->best_k[i] = -1;
    }
}

void heuristic_extension_add (HeuristicExtension* he, int k, int time)
{
    he->k[he->k_cur_size] = k;
    he->k_times[he->k_cur_size] = time;
    he->k_cur_size++;
}

void heuristic_extension_calculate_best_extensions (HeuristicExtension* he)
{
    assert (he->best_k_cur_size == 0);
    he->best_k_cur_size = min(he->extensions_threshold, he->k_cur_size);
    assert (he->best_k_cur_size >= 1);
    
    // selection sort algorithm to find the he->best_k_cur_size best extensions
    for (int i = 0; i < he->best_k_cur_size; i++) {
        int min_idx = i;
        for (int j = i+1; j < he->k_cur_size; j++) {
            if (he->k_times[j] < he->k_times[min_idx]) {
                min_idx = j;
            }
        }
        
        int tmp = he->k[min_idx];
        he->k[min_idx] = he->k[i];
        he->k[i] = tmp;
        
        tmp = he->k_times[min_idx];
        he->k_times[min_idx] = he->k_times[i];
        he->k_times[i] = tmp;
    }

    // for (int i=0; i < he->best_k_cur_size; i++){
    //     he->best_k[i] = he->k[i];
    // }
    memcpy(he->best_k, he->k, he->best_k_cur_size * sizeof(int));

    // checking invariants
    for (int i = 1; i < he->best_k_cur_size; i++) {
        assert(he->k_times[i] >= he->k_times[i-1]);
    }
    for (int i = he->best_k_cur_size; i < he->k_cur_size; i++) {
        assert(he->k_times[i] >= he->k_times[he->best_k_cur_size-1]);
    }
}