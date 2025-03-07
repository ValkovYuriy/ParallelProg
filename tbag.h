//////////////////////////////////////////////////////////////
//	файл: tbag.h											//
//	Модуль 'портфель задач' для POSIX pthread		      	//
//	Copyright (c) 2010-2013 Востокин Сергей Владимирович	//
//////////////////////////////////////////////////////////////

#ifndef _TASK_BAG_RUN_TIME
#define _TASK_BAG_RUN_TIME

#include <pthread.h>

namespace TEMPLET {

void *tFunc(void *p);

class TBag{
	friend void *tFunc(void *p);
public:
	class Task{
	public:
		virtual~Task(){}
		void send(void*,size_t){}
		void recv(void*,size_t){}
	};
public:
	TBag(int num_prc,int argc=0, char* argv[]=0);
	virtual ~TBag();
	virtual Task* createTask()=0;
	
	void run();

	virtual bool if_job()=0;
	virtual void put(Task*)=0;
	virtual void get(Task*)=0;
	virtual void proc(Task*)=0;

	double speedup(){return nproc;};
	double duration(){return _duration;};

private:
	Task** task;
	pthread_t* thread;
	pthread_attr_t attr;
	int nproc;
	volatile int c_active;
	volatile int cur_task;
	pthread_cond_t await;
	pthread_mutex_t cs;
	double _duration;
};

}
#endif
