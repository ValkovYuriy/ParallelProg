#include <iostream>
#include <queue>
#include <tchar.h>
#include "tbag.h"
#include <chrono>
#include <random>

const int N = 1000000;
const int P = 10;
int arr[N];

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

    if (j > 0) b->taskQueue.push(task(a, j + 1));
    if (size > i)b->taskQueue.push(task(a + i, size - i));
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
    bag b;

    explicit TaskBag(int num_prc) : TBag(num_prc) {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::mt19937 generator(seed);
        std::uniform_int_distribution<int> distribution(1, N);
        for (int & i : arr) {
            i = distribution(generator);
        }
        qSort0(&b, arr, N);
    }

    ~TaskBag() override = default;

    TBag::Task *createTask() override { return new TaskBagTask; }

    //std::queue<struct task> taskQueue;

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
    TaskBag bag(P);
    bag.run();
    //std::cout<<"\nspeedup = "<<bag.speedup();//1-при логической отладке,

    //for (int i = 0; i < N; i++) std::cout << i << ") " << arr[i] << '\n';
    std::cout << "\nduration = " << bag.duration() << " sec\n";
    return 0;
}

