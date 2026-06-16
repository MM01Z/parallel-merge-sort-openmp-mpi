#include "csv_utils.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <iostream>

void generateCSV(const std::string& filename, int N) {
    std::ofstream out(filename);
    srand(time(0));

    for (int i = 0; i < N; i++) {
        out << rand() % 100;
        if (i != N - 1) out << ",";
    }

    out.close();
    std::cout << "Generated CSV: " << filename
              << " (" << N << " numbers)\n";
}

std::vector<int> readCSV(const std::string& filename) {
    std::ifstream in(filename);
    std::vector<int> arr;
    std::string line, val;

    while (getline(in, line)) {
        std::stringstream ss(line);
        while (getline(ss, val, ',')) {
            arr.push_back(std::stoi(val));
        }
    }
    return arr;
}
