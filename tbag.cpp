//////////////////////////////////////////////////////////////
//	файл: tbag.cpp											//
//	Модуль 'портфель задач' для WINDOWS API			      	//
//	Copyright (c) 2010-2013 Востокин Сергей Владимирович	//
//////////////////////////////////////////////////////////////

#include "tbag.h"
#include <cassert>

namespace TEMPLET {

    TBag::TBag(int num_prc)
    {
        nproc=num_prc;

        task=new Task*[nproc];
        thread=new HANDLE[nproc];

        InitializeCriticalSection(&cs);
        await=CreateEvent(nullptr,FALSE,FALSE,nullptr);//auto,non-signaled
        assert(await);

        _duration=0.0;
    }
    DWORD WINAPI tFunc(LPVOID p)
    {
        TBag* b=(TBag*)p;
        TBag::Task* task;

        EnterCriticalSection(&b->cs);
        task=b->task[b->cur_task+=1];
        for(;;){
            while(!b->if_job()){
                if(!b->c_active){
                    SetEvent(b->await);
                    LeaveCriticalSection(&b->cs);
                    return 0;
                }
                LeaveCriticalSection(&b->cs);
                WaitForSingleObject(b->await,INFINITE);
                EnterCriticalSection(&b->cs);
            }
            b->get(task);
            b->c_active+=1;
            LeaveCriticalSection(&b->cs);

            b->proc(task);

            EnterCriticalSection(&b->cs);
            b->c_active-=1;
            b->put(task);
            SetEvent(b->await);
        }
    }
    TBag::~TBag()
    {
        delete task;
        delete thread;
        DeleteCriticalSection(&cs);
        CloseHandle(await);
    }

    void TBag::run()
    {
        DWORD id;

        cur_task=0;
        c_active=0;

        for(int i=0;i<nproc;i++){
            task[i]=createTask();
            thread[i]=CreateThread(nullptr,0,tFunc,this,0,&id);
            assert(thread[i]&&task[i]);
        }

        //run
        LARGE_INTEGER frequency;
        LARGE_INTEGER t1,t2;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&t1);

        WaitForMultipleObjects(nproc,thread,TRUE,INFINITE);

        QueryPerformanceCounter(&t2);
        _duration=(double)(t2.QuadPart-t1.QuadPart)/(double )frequency.QuadPart;

        for(int i=0;i<nproc;i++){
            CloseHandle(thread[i]);
            delete task[i];
        }
    }
}
