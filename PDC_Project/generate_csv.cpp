#include <mpi.h>
#include <iostream>

int main(int argc, char** argv) {

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::cout << "Hello 1 from process " << std::endl;

    return 0;
}
