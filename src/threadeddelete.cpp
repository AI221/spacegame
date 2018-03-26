#include "threadeddelete.h"


//Unit testing
unsigned short numDeleted = 0;
pthread_mutex_t muhlock = PTHREAD_MUTEX_INITIALIZER;
class testerclass
{
	public:
		testerclass(){}
		~testerclass()
		{
			pthread_mutex_lock(&muhlock);
			numDeleted = numDeleted+1;
			pthread_mutex_unlock(&muhlock);
		}

};
bool GE_TEST_ThreadedDelete()
{
	//test by queueing some elements for deletion, then delete the threaded delete itself.
	//this will cause the threaded delete to run a gc cycle, then wait for all threads to finish up and die.
	bool passedAll = true;	
	{
		GE_ThreadedDelete<testerclass>* deleter = new GE_ThreadedDelete<testerclass>(4);

		deleter->queue_delete(new testerclass());
		deleter->queue_delete(new testerclass());
		deleter->queue_delete(new testerclass());
		deleter->queue_delete(new testerclass());
		//deleter->force_cylce();
		delete deleter; //force gc collection & wait for end

		GE_TEST_Log("Ensure proper deletion with 4 threads\n");
		pthread_mutex_lock(&muhlock);
		GE_TEST_ASSERT(GE_StringifyNumber,numDeleted,4,==);
		pthread_mutex_unlock(&muhlock);
	}
	{
		numDeleted = 0;
		auto deleter = new GE_ThreadedDelete<testerclass>(2);

		deleter->queue_delete(new testerclass());
		deleter->queue_delete(new testerclass());
		deleter->queue_delete(new testerclass());
		deleter->queue_delete(new testerclass());
		//deleter->force_cylce();
		delete deleter; //force gc collection & wait for end

		GE_TEST_Log("Ensure proper deletion with 2 threads\n");
		pthread_mutex_lock(&muhlock);
		GE_TEST_ASSERT(GE_StringifyNumber,numDeleted,4,==);
		pthread_mutex_unlock(&muhlock);
	}
	{
		numDeleted = 0;
		auto deleter = new GE_ThreadedDelete<testerclass>(1);

		deleter->queue_delete(new testerclass());
		deleter->queue_delete(new testerclass());
		deleter->queue_delete(new testerclass());
		deleter->queue_delete(new testerclass());
		//deleter->force_cylce();
		delete deleter; //force gc collection & wait for end

		GE_TEST_Log("Ensure proper deletion with 1 thread\n");
		pthread_mutex_lock(&muhlock);
		GE_TEST_ASSERT(GE_StringifyNumber,numDeleted,4,==);
		pthread_mutex_unlock(&muhlock);
	}

	return passedAll;
}
