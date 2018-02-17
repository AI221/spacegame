/*!
 * @file
 * 
 * @author Jackson McNeill
 *
 * Provides a thread-safe event transferer
 */
#pragma once

union SDL_Event;

#include <pthread.h>
#include <stack>




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

