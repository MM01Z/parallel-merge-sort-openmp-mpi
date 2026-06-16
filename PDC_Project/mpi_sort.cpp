#include "mpi_sort.h"
#include "csv_utils.h"
#include "mergesort.h"

#include <mpi.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

static bool isSorted(const std::vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); i++){
        //std::cout << arr[i-1] << " ";
        if (arr[i] < arr[i - 1]) return false;
    }
    return true;
}

static std::vector<int> mergeVectors(const std::vector<int>& A,
                                     const std::vector<int>& B) {
    std::vector<int> C;
    C.reserve(A.size() + B.size());

    size_t i = 0, j = 0;
    while (i < A.size() && j < B.size()) {
        if (A[i] <= B[j]) C.push_back(A[i++]);
        else C.push_back(B[j++]);
    }
    while (i < A.size()) C.push_back(A[i++]);
    while (j < B.size()) C.push_back(B[j++]);

    return C;
}

void runMPI(const std::string& file, int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<int> data;
    int n = 0;

    if (rank == 0) {
        data = readCSV(file);
        n = data.size();
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int base = n / size;
    int rem  = n % size;

    int local_n = base + (rank < rem ? 1 : 0);

    std::vector<int> local(local_n);

    std::vector<int> counts, displs;
    if (rank == 0) {
        counts.resize(size);
        displs.resize(size);
        int offset = 0;
        for (int i = 0; i < size; i++) {
            counts[i] = base + (i < rem ? 1 : 0);
            displs[i] = offset;
            offset += counts[i];
        }
    }

    MPI_Scatterv(
        rank == 0 ? data.data() : nullptr,
        counts.data(), displs.data(), MPI_INT,
        local.data(), local_n, MPI_INT,
        0, MPI_COMM_WORLD
    );

    double start = MPI_Wtime();

    if (local_n > 0)
        mergeSort(local, 0, local_n - 1);

    //tree based merge
    int step = 1;
    while (step < size) {
        if (rank % (2 * step) == 0) {
            int partner = rank + step;
            if (partner < size) {
                int recv_n;
                MPI_Recv(&recv_n, 1, MPI_INT, partner, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                std::vector<int> recv_buf(recv_n);
                MPI_Recv(recv_buf.data(), recv_n, MPI_INT,
                         partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                local = mergeVectors(local, recv_buf);
                local_n = local.size();
            }
        } else {
            int partner = rank - step;
            MPI_Send(&local_n, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            MPI_Send(local.data(), local_n, MPI_INT, partner, 0,
                     MPI_COMM_WORLD);
            break;
        }
        step *= 2;
    }
    //tree based merge

    double T_mpi = MPI_Wtime() - start;

    if (rank == 0) {
        double T_seq;
        std::ifstream in("baseline.txt");
        in >> T_seq;
        in.close();

        double speedup = T_seq / T_mpi;
        double efficiency = speedup / size;

        //result.csv
        std::ofstream csv("results.csv", std::ios::app);
        csv << "mpi," << size << "," << data.size() << ","
            << T_mpi << "," << speedup << "," << efficiency << "\n";
        csv.close();
        //result.csv

        std::cout << "MPI processes: " << size << "\n";

        std::cout << "Base chunk size: " << base << "\n";
        if (rem > 0)
        std::cout << "Extra elements distributed to first " << rem << " processes\n";

        std::cout << "MPI Time: " << T_mpi << " s\n";
        std::cout << "Speedup: " << speedup << "\n";
        std::cout << "Efficiency: " << efficiency << "\n";
        std::cout << "Sorted: " << (isSorted(local) ? "YES" : "NO") << "\n\n";
    }

    MPI_Finalize();
}
