//////////////////////////////////////////////////////////////
//	����: tbag.cpp											//
//	������ '�������� �����' ��� POSIX pthread		      	//
//	Copyright (c) 2010-2013 �������� ������ ������������	//
//////////////////////////////////////////////////////////////

#include "tbag.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef WIN32
#include <sys/time.h>
#else
#include <windows.h>
#endif

namespace TEMPLET {

TBag::TBag(int num_prc,int argc, char* argv[])
{
	nproc=num_prc;
	
	task=new Task*[nproc];
	thread=new pthread_t[nproc];
	
	pthread_mutex_init(&cs,NULL);
	pthread_cond_init(&await,NULL);
	
	_duration=0.0;
}

TBag::~TBag()
{
	delete task;
	delete thread;
	pthread_mutex_destroy(&cs);
	pthread_cond_destroy(&await);
}
	
void TBag::run()
{
	cur_task=0;
	c_active=0;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	for(int i=0;i<nproc;i++){
		task[i]=createTask();
		int res=pthread_create(&thread[i],&attr,tFunc,this);
		assert(res==0 && task[i]);
	}
	
	//run
#ifndef WIN32
	struct timeval tim1,tim2;   
    gettimeofday(&tim1, NULL);   
#else
	LARGE_INTEGER frequency;
	LARGE_INTEGER t1,t2;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&t1);
#endif

	for(int i=0;i<nproc;i++)pthread_join(thread[i], NULL);

#ifndef WIN32	
	gettimeofday(&tim2, NULL);
	_duration=tim2.tv_sec+(tim2.tv_usec/1000000.0)-tim1.tv_sec+(tim1.tv_usec/1000000.0); 
#else
	QueryPerformanceCounter(&t2);
	_duration=(double)(t2.QuadPart-t1.QuadPart)/frequency.QuadPart;
#endif
	
	for(int i=0;i<nproc;i++)delete task[i];

	pthread_attr_destroy(&attr);
}

void* tFunc(void* p)
{
	TBag* b=(TBag*)p;
	TBag::Task* task;

	pthread_mutex_lock(&b->cs);
	task=b->task[b->cur_task++];
	for(;;){
		while(!b->if_job()){
			if(!b->c_active){
				pthread_cond_signal(&b->await);
				pthread_mutex_unlock(&b->cs);
				return 0;
			}
			pthread_cond_wait(&b->await,&b->cs);
		}
		b->get(task);
		b->c_active++;
		pthread_mutex_unlock(&b->cs);

		b->proc(task);
		
		pthread_mutex_lock(&b->cs);
		b->c_active--;
		b->put(task);
		pthread_cond_signal(&b->await);
	}
	return 0;
}
}
