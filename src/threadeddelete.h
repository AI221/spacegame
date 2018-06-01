/*!
 * @file
 *
 * @author Jackson McNeill
 *
 * Provides multi-threaded memory freeing. This gives a performance benefit by running delete on a seperate thread, and preventing the main code from having to wait on the delete.
 * 
 * Once something has been queued for deletion, it is undefined behaviour to access memory from it. It will likely be arround for a little while, but it is 
 * completely undefined as to when it will be deleted.
 */
#pragma once

#include <stdio.h>
#include <pthread.h>

#include <vector>
#include <stack>
#include <thread>  //Although I use pthreads, I need this to get thread count on the computer.
#include <algorithm>
#include <cmath>
#include <atomic>

#include "GeneralEngineCPP.h"

template<class T>
class GE_ThreadedDelete
{
	public:
		GE_ThreadedDelete();
		GE_ThreadedDelete(unsigned int thread_count);
		~GE_ThreadedDelete();
		/*!
		 * Lock all delete threads
		 */
		void lock();
		/*!
		 * Unlock all delete threads
		 */
		void unlock();
		/*!
		 * Queue a delete - must happen while locked
		 */
		void queue_delete(T* object);
		/*!
		 * Force a delete cycle - must happen while unlocked
		 */
		void force_cylce();




		unsigned int thread_count;
		std::vector<char> resume_flags;
		std::vector<pthread_cond_t> resume_conds;
		std::vector<std::stack<T*>> work;
		std::vector<pthread_mutex_t> work_mutexs;
		std::vector<pthread_mutex_t> cond_mutexs;
		std::vector<pthread_t> threads;
		std::vector<std::atomic<bool>*> threads_on;
		unsigned int thread_to_do_work = 0;

		std::stack<T*> work_list_nothreadaccess_unsorted;


		std::atomic<bool> on;

	private:
		void push_work_to_threads();
		void _construct_step_2();
};



template<class T>
struct thread_info
{
	unsigned int thread_number;
	GE_ThreadedDelete<T>* host;
};


template<class T>
void* thread(void* thread_data)
{
	unsigned int thread_number;
	GE_ThreadedDelete<T>* host;
	unsigned int jbcount = 0;

	//assign our thread number and host
	{
		auto my_info = static_cast<thread_info<T>*>(thread_data);
		thread_number = my_info->thread_number;
		host = my_info->host;
		delete my_info;
	}


	//We want to run this through at least once--let's say the user creates, assigns work, and then destroys the threaded deleter so fast that not all threads have had a chance to check if the host is on. This is also in an effort to preserve the rare 'do-while' loop species.
	do {
		//wait to be resumed
		pthread_mutex_lock(&(host->cond_mutexs[thread_number]));
		while(!(host->resume_flags[thread_number]))
		{
			
			pthread_cond_wait(&(host->resume_conds[thread_number]),&(host->cond_mutexs[thread_number]));
		}
		//printf(" RESUME THREAD %d\n",thread_number);
		//only run this once
		(host->resume_flags[thread_number]) = false;
		pthread_mutex_unlock(&(host->cond_mutexs[thread_number]));

		//delete everything in our work queue
		pthread_mutex_lock(&host->work_mutexs[thread_number]);
		GE_TEST_StartBenchmark();
		while(!host->work[thread_number].empty())
		{
			jbcount++;
			delete host->work[thread_number].top();
			host->work[thread_number].pop();
		}
		printf("time took %f\n",GE_TEST_BenchmarkResult());
		//printf("Thread %d did some work #%d\n",thread_number,jbcount);
		pthread_mutex_unlock(&host->work_mutexs[thread_number]);

	} while (host->on.load());
	host->threads_on[thread_number]->store(false);
	return static_cast<void*>(NULL);
}
template<class T>
GE_ThreadedDelete<T>::GE_ThreadedDelete()
{
	this->thread_count = std::thread::hardware_concurrency();
	_construct_step_2();
}
template<class T>
GE_ThreadedDelete<T>::GE_ThreadedDelete(unsigned int thread_count)
{
	this->thread_count = thread_count;
	_construct_step_2();
}
template<class T>
void GE_ThreadedDelete<T>::_construct_step_2()
{
	on.store(true);
	//printf("Spawning %d threads\n",thread_count);

	for(unsigned int i=0;i != thread_count;i++)
	{
		pthread_cond_t new_cond;
		pthread_mutex_t new_mutex;
		pthread_mutex_t new_cond_mutex;
		resume_flags.push_back(false);

		resume_conds.push_back(new_cond);
		pthread_cond_init(&resume_conds[i],NULL);

		work.push_back(std::stack<T*>());

		work_mutexs.push_back(new_cond_mutex);
		pthread_mutex_init(&work_mutexs[i],NULL);

		cond_mutexs.push_back(new_mutex);
		pthread_mutex_init(&cond_mutexs[i],NULL);
		
		threads_on.push_back(new std::atomic<bool>(true));
	}

	for(unsigned int i=0;i != thread_count;i++)
	{
		pthread_t new_thread;
		thread_info<T>* new_thread_info = new thread_info<T>{i,this}; //delete 'd by new thread. easiest way to work this in a threadsafe manner
		pthread_create(&new_thread,nullptr,thread<T>,static_cast<void*>(new_thread_info));
		threads.push_back(new_thread);
	}

}
template<class T>
GE_ThreadedDelete<T>::~GE_ThreadedDelete()
{
	on.store(false);
	force_cylce();
	//wait for all threads to finish - atomic operation
	bool stillWorking = true;
	while (stillWorking)
	{
		stillWorking = false;
		//printf("more\n");
		for (unsigned int i=0;i != thread_count;i++)
		{
			stillWorking = threads_on[i]->load() or stillWorking;
			//printf("thrad #%d is on? %d result %d\n",i,threads_on[i]->load(),stillWorking);
		}
	}
	//cleanup
	for (unsigned int i=0;i != thread_count;i++)
	{
		pthread_mutex_destroy(&work_mutexs[i]);
		pthread_mutex_destroy(&cond_mutexs[i]);
		pthread_cond_destroy(&resume_conds[i]);
		delete threads_on[i];
		pthread_join(threads[i], nullptr); //join our completed thread -- so that it will be cleaned up
	}
}
template<class T>
void GE_ThreadedDelete<T>::lock()
{
	for (unsigned int i=0;i != thread_count;i++)
	{
		pthread_mutex_lock(&work_mutexs[i]);
	}
}
template<class T>
void GE_ThreadedDelete<T>::unlock()
{
	for (unsigned int i=0;i != thread_count;i++)
	{
		pthread_mutex_unlock(&work_mutexs[i]);
	}
}

template<class T>
void GE_ThreadedDelete<T>::push_work_to_threads()
{
	//lock all threads
	lock();
	//assign the work to various threads
	//The effeciency of this could be improved by dividing into mostly equal parts then tacking the remainder onto the last thread. However, it likely give negligable performance benefits in use.
	while(!work_list_nothreadaccess_unsorted.empty())
	{
		thread_to_do_work = wraparround_clamp(thread_to_do_work+1,thread_count);	
		//printf("assign to %d\n",thread_to_do_work);

		work[thread_to_do_work].push(work_list_nothreadaccess_unsorted.top());
		work_list_nothreadaccess_unsorted.pop();
	}
	//unlock all threads
	unlock();
}

template<class T>
void GE_ThreadedDelete<T>::queue_delete(T* object)
{
	work_list_nothreadaccess_unsorted.push(object);
}
#include <SDL2/SDL.h>
template<class T>
void GE_ThreadedDelete<T>::force_cylce()
{
	push_work_to_threads();
	for (unsigned int i=0;i != thread_count;i++)
	{
		//printf("try resume %d\n",i);
		pthread_mutex_lock(&cond_mutexs[i]);
		resume_flags[i] = true;
		pthread_cond_signal(&resume_conds[i]);
		pthread_mutex_unlock(&cond_mutexs[i]);
		
		/*while (resume_flags[i])
		{
		pthread_cond_signal(&resume_conds[i]);
		}*/
	}
}


bool GE_TEST_ThreadedDelete();
