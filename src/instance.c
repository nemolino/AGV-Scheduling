#include "instance.h"
    
void instance_init_U(Instance* ins);

Instance* instance_create(char* file_path)
{
    FILE *fp = fopen(file_path, "r");
    if (fp == NULL) {
        printf("Cannot open file!\n");
        exit(EXIT_FAILURE);
    }

    Instance* ins = (Instance*)safe_malloc(sizeof(Instance));

    fscanf(fp, "%d", &(ins->W));
    fscanf(fp, "%d", &(ins->J));

    ins->p = (int *)safe_calloc(ins->W+1, sizeof(int));
    for (int i = 1; i < ins->W+1; i++) {
        fscanf(fp, "%d", &(ins->p)[i]);
    }
    
    ins->t = (int**)safe_malloc((ins->W+2) * sizeof(int*));
    (ins->t)[0] = (int*)safe_malloc((ins->W+2) * (ins->W+2) * sizeof(int));
    for(int i = 1; i < (ins->W+2); i++) {
        (ins->t)[i] = (ins->t)[0] + i * (ins->W+2);
    }
    for(int i = 0; i < (ins->W+2); i++) {
        for(int j = 0; j < (ins->W+2); j++) {
            fscanf(fp, "%d", &(ins->t)[i][j]);
        }
    }
    
    fclose(fp);

    instance_init_U(ins);

    return ins;
}

void instance_print(Instance* ins)
{
    printf("W =\t%d\n", ins->W);
    printf("J =\t%d\n", ins->J);
    printf("U =\t%d\n", ins->U);
    printf("p =\t");
    for (int i = 1; i < ins->W+1; i++) {
        printf("%d ", (ins->p)[i]);
    }
    printf("\n");

    printf("t =\n");
    for (int i = 0; i < ins->W+2; i++) {
        printf("\t");
        for (int j = 0; j < ins->W+2; j++) {
            printf("%d ", ins->t[i][j]);
        }
        printf("\n");
    }
}

void instance_destroy(Instance* ins)
{   
    safe_free(ins->p);
    ins->p = NULL;
    safe_free(ins->t[0]);
    ins->t[0] = NULL;
    safe_free(ins->t);
    ins->t = NULL;
    safe_free(ins);
    ins = NULL;
}

void instance_init_U(Instance* ins)
{
    int tot_processing_time = 0;
	for (int i = 1; i < ins->W+1; i++) {
		tot_processing_time += (ins->p)[i];
	}
	tot_processing_time *= ins->J;

	int tot_forward_travel_time = 0;
	for (int i = 0; i < ins->W+1; i++) {
		tot_forward_travel_time += (ins->t)[i][i+1];
	}
	tot_forward_travel_time *= ins->J;

	int tot_backward_travel_time = (ins->t)[ins->W+1][0] * (ins->J-1);

	ins->U = tot_processing_time + tot_forward_travel_time + tot_backward_travel_time;
}