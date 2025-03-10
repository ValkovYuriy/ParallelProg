//////////////////////////////////////////////////////////////
//	файл: tbag.h											//
//	Модуль 'портфель задач' для WINDOWS API			      	//
//	Copyright (c) 2010-2013 Востокин Сергей Владимирович	//
//////////////////////////////////////////////////////////////

#ifndef TASK_BAG_RUN_TIME
#define TASK_BAG_RUN_TIME

#include <windows.h>

namespace TEMPLET {

    class TBag{
        friend DWORD WINAPI tFunc(LPVOID);
    public:
        class Task{
        public:
            virtual~Task()= default;

            [[maybe_unused]] void send(void*,size_t){}

            [[maybe_unused]] void recv(void*,size_t){}
        };
    public:
        explicit TBag(int num_prc);
        virtual ~TBag();
        virtual Task* createTask()=0;

        void run();
        virtual bool if_job()=0;
        virtual void put(Task*)=0;
        virtual void get(Task*)=0;
        virtual void proc(Task*)=0;

        [[maybe_unused]] [[nodiscard]] double speedup() const{return nproc;};
        [[nodiscard]] double duration() const{return _duration;};

    private:
        Task** task;
        HANDLE* thread;
        int nproc;
        volatile int c_active{};
        volatile int cur_task{};
        HANDLE await;
        CRITICAL_SECTION cs{};
        double _duration;
    };

}
#endif
