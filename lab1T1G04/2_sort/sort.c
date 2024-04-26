#include <time.h>
#include <stdlib.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include "sort.h"
#include <string.h>

int sort(int *array, int n) {
    int i, j, tmp;

    for (i=1;i<n;i++) {
        tmp = array[i];
        for(j=i-1;j >= 0 && array[j] > tmp;j--) {
            array[j+1] = array[j];
        }
        array[j+1] = tmp;
    }
}

int sort_openmp(int *array, int n) {
    int tmp_array[n];
    memcpy(tmp_array, array, sizeof(int)*n);
    int head_i[_NUM_THREADS];
    int tail_i[_NUM_THREADS];
    int chunksize = n/_NUM_THREADS;
    int i, j, tmp;

    #pragma omp parallel num_threads(_NUM_THREADS) private(i, j, tmp)
    {
        int id = omp_get_thread_num();
        int start = chunksize*id;
        int final = start + chunksize;
        for (i=(start + 1); i<final; i++) {
            tmp = tmp_array[i];
            for(j=i-1;j >= start && tmp_array[j] > tmp;j--) {
                tmp_array[j+1] = tmp_array[j];
            }
            tmp_array[j+1] = tmp;
        }

        head_i[id] = start;
        tail_i[id] = final - 1;
    }

    //sequential part
    int min_value = INT32_MAX;
    int min_idx = 0;

    for (int j = 0; j < n; j++) {
        for (int i = 0; i < _NUM_THREADS; i++) {
            if (min_value > tmp_array[head_i[i]]) {
                min_value = tmp_array[head_i[i]];
                min_idx = i;
            }
        }
        array[j] = min_value;
        //we are checking that the value is inside the correct boundaries
        if (tail_i[min_idx] > head_i[min_idx]){
            head_i[min_idx]++;
        } else {
            tmp_array[tail_i[min_idx]] = INT32_MAX;
        }
        min_value = INT32_MAX;
    }

}

void fill_array(int *array, int n) {
    int i;
    srand(time(NULL));
    for(i=0; i < n; i++) {
        array[i] = rand()%n;
    }
}
