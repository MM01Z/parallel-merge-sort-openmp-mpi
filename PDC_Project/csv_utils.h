#ifndef CSV_UTILS_H_INCLUDED
#define CSV_UTILS_H_INCLUDED

#pragma once
#include <vector>
#include <string>

void generateCSV(const std::string& filename, int N);
std::vector<int> readCSV(const std::string& filename);


#endif // CSV_UTILS_H_INCLUDED
