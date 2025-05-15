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

    // for (int i=0; i < he->extensions_max; i++){
    //     printf("%d ", he->k[i]);
    // }
    // printf("\n");
    // for (int i=0; i < he->extensions_max; i++){
    //     printf("%d ", he->k_times[i]);
    // }
    // printf("\n");


    // for (int i=0; i < he->best_k_cur_size; i++){
    //     assert (he->k[he->k_cur_size-1-i] >= 0 && he->k[he->k_cur_size-1-i] <= W+1);
    //     he->best_k[i] = he->k[he->k_cur_size-1-i];
    // }


    // TODO
    // fixare il fatto che prendere le migliori estensioni al posto 
    // di estensioni casuali rompe tutto con un segfault, 
    // capire se il problema è qui oppure altrove

    
    // Selection Sort: Sort he->k based on increasing values of he->k_times
    for (int i = 0; i < he->k_cur_size - 1; i++) {
        int min_idx = i;

        // Find index of the smallest k_times[i, k_cur_size)
        for (int j = i + 1; j < he->k_cur_size; j++) {
            if (he->k_times[j] < he->k_times[min_idx]) {
                min_idx = j;
            }
        }

        // Swap he->k[min_idx] and he->k[i]
        int temp_k = he->k[min_idx];
        he->k[min_idx] = he->k[i];
        he->k[i] = temp_k;

        // Swap he->k_times[min_idx] and he->k_times[i] to maintain correct association
        int temp_k_time = he->k_times[min_idx];
        he->k_times[min_idx] = he->k_times[i];
        he->k_times[i] = temp_k_time;
    }

    // Ensure sorted order
    for (int i = 1; i < he->k_cur_size; i++) {
        assert(he->k_times[i] >= he->k_times[i-1]); // Ensure increasing order
    }

    for (int i=0; i < he->best_k_cur_size; i++){
        he->best_k[i] = he->k[i];
    //     printf("%d ", he->k_times[i]);
    }
    // printf("\n");
    


    // for (int i=0; i < he->best_k_cur_size; i++){
    //     printf("%d ", he->best_k[i]);
    // }
    // printf("\n");
    // printf("\n");
    

    /*
    for (int i=0; i < he->best_k_cur_size; i++){

        // finding minimum in he->k_times[i,he->k_cur_size)
        assert (he->k_times[i] != -1);
        int min_time = he->k_times[i];
        int min_time_idx = i;
        for (int j=i+1; j < he->k_cur_size; j++){
            assert (he->k_times[j] != -1);
            if (he->k_times[j] < min_time){
                min_time = he->k_times[j];
                min_time_idx = j;
            }
        }
        he->best_k[i] = he->k[min_time_idx];

        int tmp = he->k[min_time_idx];
        he->k[min_time_idx] = he->k[i];
        he->k[i] = tmp;

        tmp = he->k_times[min_time_idx];
        he->k_times[min_time_idx] = he->k_times[i];
        he->k_times[i] = tmp;
    }

    // --- 
    // checking invariants
    for (int i=1; i < he->best_k_cur_size; i++){
        assert (he->best_k[i] == he->k[i]);
        //assert (he->k_times[he->best_k[i]] >= he->k_times[he->best_k[i-1]]);
    }
    for (int i=he->best_k_cur_size; i < he->extensions_threshold; i++){
        assert (he->best_k[i] == -1);
    }
    for (int i=he->best_k_cur_size; i < he->k_cur_size; i++){
        assert (he->k_times[he->best_k_cur_size-1] <= he->k_times[i]);
    }
    */
}