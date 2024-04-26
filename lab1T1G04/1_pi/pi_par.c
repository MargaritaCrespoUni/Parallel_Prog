#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double compute_pi(int N) {
    double pi = 0.0;
    double delta_x = 1.0 / N;

    double height;
    double *sum = (double*)malloc(sizeof(double) * omp_get_max_threads());
    int n_threads = omp_get_num_threads();


    #pragma omp parallel
    {
        double x;

        int id = omp_get_thread_num();

        int first = N/n_threads*id;
        int last = N/n_threads*(id + 1);

        if (last > N) last = N;
        double foo = 0.0;
        for (int i = first; i < last; i++) {
            x = (i + 0.5) / N;
            height = 4 / (1 + x * x);
            foo += delta_x * height;
        }
        sum[id] = foo;

    }

    for (int i = 0; i < omp_get_max_threads(); i++) {
        pi += sum[i];
    }

    free(sum);
    return pi;
}

int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);
    double start_time = omp_get_wtime();
    double pi = compute_pi(N);
    double end_time = omp_get_wtime();

    double runtime = end_time - start_time;

    printf("\nPi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime);
}
