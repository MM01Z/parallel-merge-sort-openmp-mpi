#include "openmp_sort.h"
#include "csv_utils.h"
#include "mergesort.h"
#include <omp.h>
#include <fstream>
#include <iostream>
#include <vector>

// cutoff to stop creating tasks at deep recursion
#define CUTOFF 250000

// Parallel merge sort using OpenMP tasks
void parallelMergeSort(std::vector<int>& arr, int l, int r) {
    if (l >= r) return;

    // If small chunk, do sequential merge sort
    if (r - l < CUTOFF) {
        mergeSort(arr, l, r);
        return;
    }

    int m = l + (r - l) / 2;

    #pragma omp task shared(arr)
    parallelMergeSort(arr, l, m);

    #pragma omp task shared(arr)
    parallelMergeSort(arr, m + 1, r);

    #pragma omp taskwait
    merge(arr, l, m, r);
}

void runOpenMP(const std::string& filename, int threads) {
    std::vector<int> arr = readCSV(filename);

    // read baseline time
    double T_seq;
    std::ifstream in("baseline.txt");
    in >> T_seq;
    in.close();

    omp_set_num_threads(threads);

    double start = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        parallelMergeSort(arr, 0, arr.size() - 1);
    }

    double T_omp = omp_get_wtime() - start;

    double speedup = T_seq / T_omp;
    double efficiency = speedup / threads;

    //result csv
    std::ofstream csv("results.csv", std::ios::app);
    csv << "omp," << threads << "," << arr.size() << "," << T_omp
        << "," << speedup << "," << efficiency << "\n";
    csv.close();
    //result csv

    std::cout << "Number of threads: " << threads << "\n";
    std::cout << "Cutoff: " << CUTOFF << "\n";
    std::cout << "OpenMP Time: " << T_omp << " s\n";
    std::cout << "Speedup: " << speedup << "\n";
    std::cout << "Efficiency: " << efficiency << "\n";

    // Optional: verify sorting
    bool sorted = true;
    for (int i = 1; i < arr.size(); i++) {
        //std::cout << "" << arr[i] << " ";
        if (arr[i] < arr[i - 1]) {
            sorted = false;
            break;
        }
    }

    if (sorted)
        std::cout << "Array is sorted correctly.\n";
    else
        std::cout << "Array is NOT sorted.\n";
}
