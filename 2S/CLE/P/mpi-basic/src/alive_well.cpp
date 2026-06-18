#include <iostream>
#include <cstring>
#include <mpi.h>

int main (int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int MSG_SIZE = 1024;
    char message[MSG_SIZE];
    memset(message, 0, MSG_SIZE);

    if (rank == 0) {
        snprintf(message, MSG_SIZE, "Alive and well (%d)!", rank);
        // Send to next process in the ring
        for (int i = 1; i < size; i++) {
            MPI_Send(message, strlen(message) + 1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            // Receive the final message from the last process
            MPI_Recv(message, MSG_SIZE, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::cout << "Message at rank 0 after iteration " << i + 1 << ": " << message << std::endl;
        }
    } else {
        // Receive from previous process
        MPI_Recv(message, MSG_SIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        snprintf(message, MSG_SIZE, "Alive and well (%d)!", rank);
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize ();
    return EXIT_SUCCESS;
}
