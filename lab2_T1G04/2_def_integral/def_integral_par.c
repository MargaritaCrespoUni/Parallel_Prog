#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

///this part is the same as before
double function(double x)
{
    /// defines the function f(x) = x * sin(x) that we want to integrate over the interval [0, X_MAX].
    return x*sin(x);
}

double exact_quadrature(double x)
{
    /// calculates the exact value of the definite integral of the function
    /// reference for comparing the numerical result
    return sin(x) - x*cos(x);
}

int main(int argc, char* argv[]){
    MPI_Init(&argc, &argv);

    int rank, size;
    ///rank: unique identifier assigned to each MPI process within the communicator
    ///size: stores the total number of MPI processes participating in the computation within the MPI communicator.

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /// Check parameters
    if(argc != 3) {
        if (rank == 0) {
            fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s N XMAX\n", argv[0]);
        }
        MPI_Finalize();
        return -1;
    }
    ///are parsed to determine the number of domains (N) into which the interval [0, X_MAX] is divided
    /// and the maximum value of x (X_MAX).
    int N = (int)strtol(argv[1], NULL, 10);
    double X_MAX = (double) strtold(argv[2], NULL);
    double deltaX = X_MAX/(double) N; ///width of each subinterval


    double start_time = MPI_Wtime(); // Start the timer

    double local_result = 0.0;
    int i;
    for (i = rank + 1; i <= N; i += size) {
        double x_middle = (i - 0.5) * deltaX;
        local_result += function(x_middle) * deltaX;
    }

    /// Reduce local results to get the global result
    double result;
    MPI_Reduce(&local_result, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    /// Compute error and print results (only by rank 0)
    if (rank == 0) {
        double exact = exact_quadrature(X_MAX);
        double error = fabs(result - exact);
        double run_time = MPI_Wtime() - start_time;
        printf("Result with N=%d is %.12lf (%.12lf, %.2e) in %lf seconds\n", N, result, exact, error, run_time);
    }

    MPI_Finalize();
    return 0;
}