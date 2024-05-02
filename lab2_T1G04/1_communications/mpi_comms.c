#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <mpi.h>

#define NUM_TEST 50
#define WAITING_TIME 1e6

void test_synchronous(int rank, int n);
void test_asynchronous(int rank, int n);

int main(int argc, char** argv)
{
    int rank;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(size <= 2) {
        if (rank == 0)
            printf("Error, this program needs to be executed with more than 2 processors.\n");
        return 1;
    }

    /// FIRST TEST - SYNCHRONOUS COMMUNICATION
    test_synchronous(rank, size);

    MPI_Barrier(MPI_COMM_WORLD); //added, do not know if it's okay

    /// FIRST TEST - ASYNCHRONOUS COMMUNICATION
    test_asynchronous(rank, size);

    MPI_Finalize();
    return 0;
}

void test_synchronous(int rank, int size)
{
    double start_time, run_time, average_time, max_time, min_time;
    int sbuf[NUM_TEST]; //array to send data
    int rbuf[NUM_TEST]; //array to receive data
    MPI_Status status; //interesting!! the other one doesn't have it

    /// TODO compute send/receive cores
    int send_core = (rank +1) % size;
    int recv_core = (rank-1 + size) % size;


    start_time = MPI_Wtime();
    for(int i = 0; i < NUM_TEST; i++) { //iterate over specified number of tests sending data synchronimously for each process
        sbuf[i] = rank; //data buffer from which data will be sent (we have to use it afterwards)

        /// TODO send
        //1 is the data items to be sent
        //MPI_INT says we are sending integers
        //MPI_COMM_WORLD represent all the processes in the MPI job
        MPI_Send(&sbuf[i], 1 , MPI_INT, send_core, 0, MPI_COMM_WORLD );


        if (i % size == rank)
            usleep(WAITING_TIME);

        /// TODO recv
        //same for the receive but we add the status as the sent has to be confirmed
        MPI_Recv(&rbuf[i], 1 , MPI_INT, recv_core, 0, MPI_COMM_WORLD , &status);


        assert(rbuf[i] == recv_core);
    }
    run_time = MPI_Wtime() - start_time; //measure time taken for each iteration


    /// TODO compute average, max and min times
    MPI_Allreduce(&run_time,&average_time,  1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&run_time,&min_time,  1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    MPI_Allreduce(&run_time,&max_time,  1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    average_time= average_time/ size;

    if (rank == 0) {
        printf("Synchronous send/receive test with %d processes and %d repetitions.\n", size, NUM_TEST);
        printf("    average: %.2lf s\n", average_time);
        printf("    min:     %.2lf s\n", min_time);
        printf("    max:     %.2lf s\n", max_time);
    }

    //MPI_Barrier(MPI_COMM_WORLD); deleted here do not know if its correct

}

void test_asynchronous(int rank, int size)
{
    double start_time, run_time, average_time, max_time, min_time;
    int sbuf[NUM_TEST];
    int rbuf[NUM_TEST];

    MPI_Request send_request, recv_request;
    MPI_Status send_status, recv_status;

    /// TODO compute send/receive cores
    int recv_core = (rank + 1) % size;
    int send_core = (rank - 1 + size) % size;


    start_time = MPI_Wtime();

    for(int i = 0; i < NUM_TEST; i++) { //specified number of tests
        sbuf[i] = rank;

        /// TODO async send
        MPI_Isend(&sbuf[i], 1, MPI_INT, send_core, 0, MPI_COMM_WORLD, &send_request);


        if (rank == i)
            usleep(WAITING_TIME); //wait for all asynchronous sends and receives to complete

        ///TODO async recv
        MPI_Irecv(&rbuf[i], 1, MPI_INT, recv_core, 0, MPI_COMM_WORLD, &recv_request);


    }


    /// TODO wait for async send/receive
    MPI_Wait(&send_request, &send_status);
    MPI_Wait(&recv_request, &recv_status);

    /// TODO

    run_time = MPI_Wtime() - start_time;

    for(int i = 0; i < NUM_TEST; i++) {
        assert(rbuf[i] == recv_core);
    }

    /// TODO compute average, max and min times

    MPI_Allreduce(&run_time, &average_time, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(&run_time, &min_time, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD);
    MPI_Allreduce(&run_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    average_time /= size;

    /// TODO

    if (rank == 0) {
        printf("Asynchronous send/receive test with %d processes and %d repetitions.\n", size, NUM_TEST);
        printf("    average: %.2lf s\n", average_time);
        printf("    min:     %.2lf s\n", min_time);
        printf("    max:     %.2lf s\n", max_time);
    }

}