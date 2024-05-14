#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

double function(double x) {
    return x * sin(x);
}

double exact_integral(double x)
{
    return sin(x) - x*cos(x);
}


int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); ///determines the rank (ID) of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &size); ///determines the number of processes in the communicator


    if (argc != 3) { ///checks if the number of command-line arguments is 3
        if (rank == 0) {
            fprintf(stderr, "Usage: %s N XMAX\n", argv[0]);
        } ///if not, the process with rank 0 prints an error message and all processes call MPI_Finalize()
        MPI_Finalize();
        return -1;
    }


    int N = (int)strtol(argv[1], NULL, 10); ///converts the first command-line argument to an integer
    double X_MAX = atof(argv[2]); /// converts the second command-line argument to a double
    double deltaX = X_MAX / (double) N; ///calculates the width of each interval
    double local_sum = 0.0;

    int local_N = N / size;
    int remainder = N % size;
    if (rank < remainder) {
        local_N += 1;
    }

    double local_start = rank * (N / size) * deltaX + (rank < remainder ? rank * deltaX : remainder * deltaX);
    double local_end = local_start + local_N * deltaX;

    double* local_integral = (double*)calloc(local_N + 1, sizeof(double));

    // Synchronize all processes before starting the timer
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();

    for (int i = 1; i <= local_N; i++) {
        double x = local_start + (i - 0.5) * deltaX;
        local_integral[i] = local_integral[i - 1] + deltaX * function(x);
    }

    local_sum = local_integral[local_N];
    double global_sum = 0.0; 
    double* all_sums = NULL;
    if (rank == 0) {
        all_sums = (double*)malloc(size * sizeof(double));
    }
    ///changed
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);


    if(rank == 0) {

        for (int i = 0; i < size; i++) {
            global_sum += all_sums[i];
        }
        free(all_sums);



        double def_result = global_sum;
        double exact = exact_integral(X_MAX);
        double error = fabs(def_result - exact);
        double run_time =  MPI_Wtime() - start_time;

        printf("Result with N=%d is %.12lf (%.12lf, %.2e) in %lf seconds\n", N, def_result, exact, error, run_time);

        /// Print integral data
        char filename_dat[100];
        sprintf(filename_dat, "indef_integral_par.dat");
        FILE* file_dat = fopen(filename_dat, "wb");
        if (file_dat != NULL) {
            fwrite(&def_result, sizeof(def_result), 1, file_dat);
            fclose(file_dat);
        }

        /// Print integral info
        char filename_info[100];
        sprintf(filename_info, "indef_integral_par.info");
        FILE* file_info = fopen(filename_info, "w");
        if (file_info != NULL) {
            fprintf(file_info, "# %s\n", "Indefinite integral version parallel");
            fprintf(file_info, "data %s\n", filename_dat);
            fprintf(file_info, "N %d\n", N);
            fprintf(file_info, "XMAX %lf\n", X_MAX);
            fprintf(file_info, "size %d\n", size);
            fclose(file_info);
        }
    }

    free(local_integral);
    MPI_Finalize();
    return 0;


}