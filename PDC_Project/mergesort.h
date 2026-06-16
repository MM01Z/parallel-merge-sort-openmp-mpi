#ifndef MERGESORT_H_INCLUDED
#define MERGESORT_H_INCLUDED

#pragma once
#include <vector>

void mergeSort(std::vector<int>& arr, int left, int right);

void merge(std::vector<int>& arr, int l, int m, int r);

#endif // MERGESORT_H_INCLUDED
