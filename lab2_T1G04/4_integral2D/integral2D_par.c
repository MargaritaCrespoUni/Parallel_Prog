#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

#define X_MAX 1.0
#define Y_MAX 1.0

int main(int argc, char* argv[])
{
    /// TODO Init MPI
    int rank, size;
    int provided;

    /// TODO
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //printf("Integral 2D: Rank %d of %d\n", rank, size);

    /// Check parameters
    if(argc != 4) {
        fprintf(stderr, "Wrong number of parameters\nUsage:\n\t%s filename Nx Ny\n", argv[0]);
        return -1;
    }
    char* filename = argv[1];
    int Nx = (int)strtol(argv[2], NULL, 10);
    int Ny = (int)strtol(argv[3], NULL, 10);

    const double deltaX = 2.0*X_MAX/(double) Nx;
    const double deltaY = 2.0*Y_MAX/(double) Ny;

    /// TODO compute rank ranges
    int rows_per_rank = Ny / size;
    int start_row = rank * rows_per_rank;
    int end_row;
    if (rank == size - 1) {
        end_row = Ny; // To handle when Ny is not perfectly divisible by size
    } else {
        end_row = start_row + rows_per_rank; // Otherwise, assign start_row + rows_per_rank to end_row
    }


    /// TODO Read data
    double* data = (double*)malloc(Nx * (end_row - start_row) * sizeof(double));
    MPI_File fh;
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    MPI_Offset offset = start_row * Nx * sizeof(double);
    MPI_File_read_at_all(fh, offset, data, Nx * (end_row - start_row), MPI_DOUBLE, MPI_STATUS_IGNORE);
    MPI_File_close(&fh);    


    double start_time = omp_get_wtime();
    /// TODO Compute integral
    double local_sum = 0.0;
    #pragma omp parallel for reduction(+:local_sum)
    for(int i = 0; i < Nx; i++) {
        for(int j = 0; j < end_row - start_row; j++) {
            //double x = -X_MAX + (i + 0.5) * deltaX;
            //double y = -Y_MAX + (start_row + j + 0.5) * deltaY;
            local_sum += data[j * Nx + i]  * deltaX * deltaY; 
        }
    }

    double result;
    MPI_Reduce(&local_sum, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double run_time = omp_get_wtime() - start_time;

    ///  Print results
    if(rank == 0)
    {
        printf("Result with N=%d M=%d size: %d threads: %d is %.12lf in %lf seconds\n", Nx, Ny,
			size, omp_get_max_threads(), result, run_time);
    }

    MPI_Finalize();
    return 0;
}
