#include "SDL.h"
#include <pthread.h>
#include <stack>


#ifndef __THREADED_EVENT_STACK_INCLUDED
#define __THREADED_EVENT_STACK_INCLUDED

class GE_ThreadedEventStack
{
	public:
		GE_ThreadedEventStack();
		~GE_ThreadedEventStack();

		bool canGetEvent();
		void giveEvent(SDL_Event event);
		SDL_Event getEvent();

		void PourInputStackToOutput();
		

	private:
		std::stack<SDL_Event> out;
		std::stack<SDL_Event> in;

		pthread_mutex_t mutex;



};

#endif //__THREADED_EVENT_STACK_INCLUDED
