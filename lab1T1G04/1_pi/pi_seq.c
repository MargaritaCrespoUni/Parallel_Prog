
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double compute_pi(int N) {
    double sum = 0.0;
    double delta_x = 1.0 / N;
    double x;
    double height;

    for (int i = 0; i < N; i++) {
        x = (i + 0.5)/N;
        height = 4/(1 + x*x);
        sum += delta_x*height;
    }

    return sum;
}

int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);
    clock_t start_time = clock();
    double pi = compute_pi(N);
    clock_t end_time = clock();

    double runtime = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("\nPi with %i steps is %.15lf in %lf seconds\n", N, pi, runtime);
}