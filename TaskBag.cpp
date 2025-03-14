#include <iostream>
#include <fstream>
#include <queue>
#include <tchar.h>
#include "tbag.h"
#include <chrono>
#include <random>

#define COUNT 1000

int N = 10000;
int P = 2;
const int T = 10;


struct task {
    int *a;
    int size;
};
struct bag {
    std::queue<task> taskQueue;
};

void qSort(int *a, int size) {
    long i = 0, j = size - 1;
    int temp, p;

    p = a[size >> 1];
    do {
        while (a[i] < p) i++;
        while (a[j] > p) j--;

        if (i <= j) {
            temp = a[i];
            a[i++] = a[j];
            a[j--] = temp;
        }
    } while (i <= j);
    if (j > 0) qSort(a, j + 1);
    if (size > i) qSort(a + i, size - i);
}

void qSort0(bag *b, int *a, int size) {
    long i = 0, j = size - 1;
    int temp, p;

    p = a[size >> 1];
    do {
        while (a[i] < p) i++;
        while (a[j] > p) j--;

        if (i <= j) {
            temp = a[i];
            a[i++] = a[j];
            a[j--] = temp;
        }
    } while (i <= j);

    if (j > 0) {
        if (j + 1 < T && b->taskQueue.size() < P)
            b->taskQueue.push(task(a, j + 1));
        else
            qSort0(b, a, j + 1);
    }
    if (size > i) {
        if (size - i < T && b->taskQueue.size() < P)
            b->taskQueue.push(task(a + i, size - i));
        else
            qSort0(b, a, size - i);
    }
}

class TaskBag : public TEMPLET::TBag {
public:
    class TaskBagTask : public TBag::Task {
    public:
        TaskBagTask() : TBag::Task() {}

        ~TaskBagTask() override = default;

        [[maybe_unused]] void send_task() {}

        [[maybe_unused]] void recv_task() {}

        [[maybe_unused]] void send_result() {}

        [[maybe_unused]] void recv_result() {}

        struct task t{};
    };

public:
    bag b{};
    double real_time_milliseconds = 0.0;

    explicit TaskBag(int num_prc, int *arr) : TBag(num_prc) {
        struct timespec start{}, end{};
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 generator(seed);
        std::uniform_int_distribution<int> distribution(1, N);
        for (int i=0; i< N; i++) {
            arr[i] = distribution(generator);
        }
        clock_gettime(CLOCK_REALTIME, &start);
        qSort0(&b, arr, N);
        clock_gettime(CLOCK_REALTIME, &end);
        double real_time = (double) (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        real_time_milliseconds = real_time * 1000;
    }

    ~TaskBag() override = default;

    TBag::Task *createTask() override { return new TaskBagTask; }


    bool if_job() override { return !b.taskQueue.empty(); }

    void put(Task *t) override {}

    void get(Task *t) override {
        auto *mt = (TaskBagTask *) t;
        mt->t = b.taskQueue.front();
        b.taskQueue.pop();
    }

    void proc(Task *t) override {
        auto *mt = (TaskBagTask *) t;
        qSort(mt->t.a, mt->t.size);
    }
};

int _tmain() {
    std::ofstream outputFile("results.csv"); // Создаем файл
    if (!outputFile.is_open()) {
        std::cerr << "File error" << std::endl;
        return 1;
    }

    // Записываем заголовки
    outputFile << "N,P,AVG_SERIAL_TIME,AVG_PARALLEL_TIME\n";
    for (; N <= 10000000; N *= 10) {
        printf("N: %d\n", N);
        int arr[N];
        double real_time_milliseconds = 0.0;
        for (int count = 0; count < COUNT; count++) {
            struct timespec start{}, end{};
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::mt19937 generator(seed);
            std::uniform_int_distribution<int> distribution(1, N);
            for (int &i: arr) {
                i = distribution(generator);
            }
            clock_gettime(CLOCK_REALTIME, &start);
            qSort(arr, N);
            clock_gettime(CLOCK_REALTIME, &end);
            double real_time = (double) (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            real_time_milliseconds += real_time * 1000;
        }
        double serial_time = real_time_milliseconds / COUNT;
        printf("AVG serial time: %f ms\n", serial_time);
        for (P = 2; P <= 8; P *= 2) {
            real_time_milliseconds = 0.0;
            printf("P: %d\n", P);
            for (int count = 0; count < COUNT; count++) {
                int *ptr = arr;
                TaskBag bag(P, ptr);
                bag.run();
                real_time_milliseconds += bag.duration() * 1000 + (double) bag.real_time_milliseconds;
            }
            double parallel_time = real_time_milliseconds / COUNT;
            printf("AVG parallel time: %f ms\n", parallel_time);
            outputFile << N << "," << P << "," << serial_time << "," << parallel_time<< "\n"; // Запись в CSV (P=0 для последовательного)
        }
    }
    outputFile.close();
    return 0;
}