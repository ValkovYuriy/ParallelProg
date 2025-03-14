#include <omp.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

#define COUNT 1000

int P = 2;
int N = 10000;

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

void qsort_parallel(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);

#pragma omp task
        qsort_parallel(arr, low, pi - 1);

#pragma omp task
        qsort_parallel(arr, pi + 1, high);
    }
}

void parallel_quick_sort(int arr[], int n) {
#pragma omp parallel num_threads(P)
    {
#pragma omp single nowait
        qsort_parallel(arr, 0, n - 1);

#pragma omp taskwait
    }
}

int main() {
    std::ofstream outputFile("results.csv"); // РЎРѕР·РґР°РµРј С„Р°Р№Р»
    if (!outputFile.is_open()) {
        std::cerr << "File error" << std::endl;
        return 1;
    }
    outputFile << "N,P,AVG_SERIAL_TIME,AVG_PARALLEL_TIME\n";
    struct timespec start{}, end{};
    for (; N <= 10000000; N *= 10) {
        printf("N: %d\n", N);
        int arr[N];
        for (P = 2; P <= 8; P *= 2) {
            printf("P: %d\n", P);
            double real_time_milliseconds = 0.0;
            for (int count = 0; count < COUNT; count++) {
                unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
                std::mt19937 generator(seed);
                std::uniform_int_distribution<int> distribution(1, N);
                for (int &i: arr) {
                    i = distribution(generator);
                }
                clock_gettime(CLOCK_REALTIME, &start);
                parallel_quick_sort(arr, N);
                clock_gettime(CLOCK_REALTIME, &end);
                double real_time = (double) (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
                real_time_milliseconds += real_time * 1000;
            }
            double parallel_time = real_time_milliseconds / COUNT;
            printf("AVG parallel time: %f ms\n", parallel_time);
            outputFile << N << "," << P << "," << "serial" << "," << parallel_time<< "\n";
        }
    }
    outputFile.close();
    std::cout << "Programm end succesfully\n";
    return 0;
}