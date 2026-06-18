#include <iostream>
#include <vector>
#include <mpi.h>
#include <cstdlib>
#include <ctime>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = size; // For simplicity, one row per process
    std::vector<int> A_flat, B_flat(N * N), C_flat(N * N);

    // Only rank 0 initializes A and B
    if (rank == 0) {
        A_flat.resize(N * N);
        srand(time(0));
        std::cout << "Matrix A:\n";
        for (int i = 0; i < N * N; ++i) {
            A_flat[i] = rand() % 100;
            std::cout << A_flat[i] << ((i + 1) % N ? " " : "\n");
        }
        std::cout << "Matrix B:\n";
        for (int i = 0; i < N * N; ++i) {
            B_flat[i] = rand() % 100;
            std::cout << B_flat[i] << ((i + 1) % N ? " " : "\n");
        }
    }

    // Scatter rows of A
    std::vector<int> recv_row(N);
    MPI_Scatter(rank == 0 ? A_flat.data() : nullptr, N, MPI_INT, recv_row.data(), N, MPI_INT, 0, MPI_COMM_WORLD);
    std::cout << "Rank " << rank << " received row: ";
    for (int i = 0; i < N; ++i) {
        std::cout << recv_row[i] << " ";
    }
    std::cout << std::endl;
    // Broadcast B to all processes
    MPI_Bcast(B_flat.data(), N * N, MPI_INT, 0, MPI_COMM_WORLD);
    std::cout << "Rank " << rank << " received matrix B.\n";
    for (int i = 0; i < N * N; ++i) {
        std::cout << B_flat[i] << ((i + 1) % N ? " " : "\n");
    }

    // Each process computes its result row
    std::vector<int> result_row(N, 0);
    for (int j = 0; j < N; ++j) {
        for (int k = 0; k < N; ++k) {
            result_row[j] += recv_row[k] * B_flat[k * N + j];
        }
    }

    // Gather all result rows to rank 0
    MPI_Gather(result_row.data(), N, MPI_INT, rank == 0 ? C_flat.data() : nullptr, N, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "Result matrix C:\n";
        for (int i = 0; i < N * N; ++i) {
            std::cout << C_flat[i] << ((i + 1) % N ? " " : "\n");
        }
    }

    MPI_Finalize();
    return 0;
}
