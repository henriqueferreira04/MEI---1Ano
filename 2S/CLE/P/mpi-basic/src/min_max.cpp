#include <iostream>
#include <cstring>
#include <mpi.h>
#include <cstdlib>
#include <ctime>

int main (int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int MSG_SIZE = 5;
    int message[MSG_SIZE];
    memset(message, 0, MSG_SIZE);

    srand(time(0) + rank); // Seed with time and rank for different streams
    std::cout << "Rank " << rank << " generated numbers: ";
    for (int i = 0; i < MSG_SIZE - 1; i++) {
        message[i] = rand() % 100;
        std::cout << message[i] << " ";
    }
    std::cout << std::endl;

    // Find local min and max
    int local_min = message[0];
    int local_max = message[0];
    for (int i = 1; i < MSG_SIZE - 1; i++) {
        if (message[i] < local_min) local_min = message[i];
        if (message[i] > local_max) local_max = message[i];
    }

    // Find global min and max
    int global_min, global_max;
    MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Global min: " << global_min << std::endl;
        std::cout << "Global max: " << global_max << std::endl;
    }

    MPI_Finalize ();
    return EXIT_SUCCESS;
}
