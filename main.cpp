#include <iostream>
#include <queue>
#include <tchar.h>
#include "tbag.h"

const int N = 10000;
const int T = 100;
const int P = 100;
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

    if (j > 0)
        if (j + 1 < T) b->taskQueue.push(task(a, j + 1));
        else qSort0(b, a, j + 1);
    if (size > i)
        if (size - i < T) b->taskQueue.push(task(a + i, size - i));
        else qSort0(b, a + i, size - i);
}

class TaskBag : public TEMPLET::TBag {
public:
    class TaskBagTask : public TBag::Task {
    public:
        TaskBagTask() : TBag::Task() {}

        virtual ~TaskBagTask() {}

        void send_task() {}

        void recv_task() {}

        void send_result() {}

        void recv_result() {}

        struct task t;
    };

public:
    bag b;

    TaskBag(int num_prc, int argc, char *argv[]) : TBag(num_prc, argc, argv) {
        for (int i = 0; i < N; i++) {
            arr[i] = rand() % N;
        }
        qSort0(&b, arr, N);
    }

    virtual ~TaskBag() {}

    TBag::Task *createTask() { return new TaskBagTask; }

    //std::queue<struct task> taskQueue;

    bool if_job() { return !b.taskQueue.empty(); }

    void put(Task *t) {}

    void get(Task *t) {
        TaskBagTask *mt = (TaskBagTask *) t;
        mt->t = b.taskQueue.front();
        b.taskQueue.pop();
    }

    void proc(Task *t) {
        TaskBagTask *mt = (TaskBagTask *) t;
        qSort(mt->t.a, mt->t.size);
    }
};

int _tmain(int argc, _TCHAR *argv[]) {
    TaskBag bag(P, argc, argv);
    bag.run();
    //std::cout<<"\nspeedup = "<<bag.speedup();//1-при логической отладке,

    //for (int i = 0; i < N; i+=10) std::cout << arr[i] << ' ';
    std::cout<<"\nduration = "<<bag.duration()<< " sec\n";

    return 0;
}

/*class TaskBag : public TEMPLET::TBag {
public:
    class TaskBagTask : public TBag::Task {
    public:
        TaskBagTask() : TBag::Task() {}

        virtual ~TaskBagTask() {}

        void send_task() {}

        void recv_task() {}

        void send_result() {}

        void recv_result() {}

        task t;
    };

public:
    TaskBag(int num_prc, int argc, char *argv[]) : TBag(num_prc, argc, argv) {
        for (int i = 0; i < N; i++) {
            arr[i] = rand() % N;
        }
        qSort0(&b, arr, N);
    }

    virtual ~TaskBag() {}

    TBag::Task *createTask() { return new TaskBagTask; }
};

std::queue<task> taskQueue;

bool if_job() { return !taskQueue.empty(); }

void put(Task *t) {}

void get(Task *t) {
    TaskBagTask *mt = (TaskBagTask *) t;
    tm->t = taskQueue.front();
    taskQueue.pop();
}*/
