#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MIN_TASK_STEPS 1024

double compute_pi_task(int N, int M, int start, int end, double delta_x, int original_N);

double compute_pi(int N, int M) {
    double delta_x = 1.0 / N;
    return compute_pi_task(N, M, 0, N, delta_x, N);
}

double compute_pi_task(int N, int M, int start, int end, double delta_x, int original_N) {
    double pi = 0.0;

    if (N <= M) {
        double height;
        double sum = 0.0;

        for (int i = start; i < end; i++) {
            double x = (i + 0.5) / original_N;
            height = 4 / (1 + x * x);
            sum += delta_x * height;
        }
        //printf("\nFrom %i to %i = %.15lf\n", start, end, sum);
        return sum;
    }

    // we di it recursively when N>M
    #pragma omp task shared(pi)
    {
        double left_pi = compute_pi_task(N / 2, M, start, start + N / 2, delta_x, original_N);
        #pragma omp atomic //atomic for += or ++ operations (manage race conditions)
        pi += left_pi;
    }

    #pragma omp task shared(pi)
    {
        double right_pi = compute_pi_task(N - (N / 2), M, start + N/2, start + N, delta_x, original_N);
        #pragma omp atomic
        pi += right_pi;
    }

    #pragma omp taskwait

    return pi;
}

int main(int argc, char *argv[]) {

    int N = atoi(argv[1]);
    int M = atoi(argv[2]);

    //if M is smaller than 1024 we update M to 1024
    if (M < MIN_TASK_STEPS){
        M = MIN_TASK_STEPS;
    }

    double start_time = omp_get_wtime();
    double pi = compute_pi(N, M);
    double end_time = omp_get_wtime();

    double runtime = end_time - start_time;

    printf("\nPi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime);

    return 0;
}
