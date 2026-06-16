#include <iostream>
#include <string>
#include <fstream>
#include <omp.h>
#include "csv_utils.h"
#include "mergesort.h"
#include "openmp_sort.h"
#include "mpi_sort.h"
#include <filesystem>


void writeCSVHeader() {
    if (!std::filesystem::exists("results.csv")) {
        std::ofstream csv("results.csv");
        csv << "model,workers,input,time,speedup,efficiency\n";
        csv.close();
    }
}


int main(int argc, char** argv) {

    writeCSVHeader();

    if (argc < 2) {
        std::cout << "Usage: program [generate|seq|omp|mpi]\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string file = "input.csv";
    int threads = 4;

    if (mode == "generate") {
        generateCSV(file, 1000000);
    }

    else if (mode == "seq") {
        auto arr = readCSV(file);

        double start = omp_get_wtime();
        mergeSort(arr, 0, arr.size() - 1);
        double T_seq = omp_get_wtime() - start;

        std::ofstream csv("results.csv", std::ios::app);
        csv << "seq,1," << arr.size() << "," << T_seq << ",1.0,1.0\n";
        csv.close();

        std::ofstream out("baseline.txt");
        out << T_seq;
        out.close();

        std::cout << "Sequential Time: " << T_seq << " s\n";
        std::cout << "Baseline saved to baseline.txt\n";
    }

    else if (mode == "omp") {
        // baseline.txt is read INSIDE runOpenMP
        int threads = (argc >= 3) ? std::stoi(argv[2]) : 4;
        runOpenMP(file, threads);
    }

    else if (mode == "mpi") {
        //Run with: mpiexec -n 4 PDC_Project.exe mpi
        runMPI(file, argc, argv);
    }

    else {
        std::cout << "Invalid argument.\n";
    }

    return 0;
}
