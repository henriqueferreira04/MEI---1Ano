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
        snprintf(message, MSG_SIZE, "I am here (%d)!", rank);
        // Send to next process in the ring
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, (rank + 1) % size, 0, MPI_COMM_WORLD);
        // Receive the final message from the last process
        MPI_Recv(message, MSG_SIZE, MPI_CHAR, (size - 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Final message at rank 0: " << message << std::endl;
    } else {
        // Receive from previous process
        MPI_Recv(message, MSG_SIZE, MPI_CHAR, (rank - 1 + size) % size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Append this process's info
        size_t len = strlen(message);
        snprintf(message + len, MSG_SIZE - len, " | I am here (%d)!", rank);
        // Send to next process in the ring
        MPI_Send(message, strlen(message) + 1, MPI_CHAR, (rank + 1) % size, 0, MPI_COMM_WORLD);
        // Optionally, print the message at each rank
        std::cout << "Message at rank " << rank << ": " << message << std::endl;
    }

    MPI_Finalize ();
    return EXIT_SUCCESS;
}
