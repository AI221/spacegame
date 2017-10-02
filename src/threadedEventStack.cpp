#include "threadedEventStack.h"


GE_ThreadedEventStack::GE_ThreadedEventStack()
{
	mutex = PTHREAD_MUTEX_INITIALIZER;
}
GE_ThreadedEventStack::~GE_ThreadedEventStack()
{

}

bool GE_ThreadedEventStack::canGetEvent()
{
	pthread_mutex_lock(&mutex);
	bool result = !out.empty();
	pthread_mutex_unlock(&mutex);
	return result;
}
void GE_ThreadedEventStack::giveEvent(SDL_Event event)
{
	pthread_mutex_lock(&mutex);
	in.push(event);
	pthread_mutex_unlock(&mutex);
}

SDL_Event GE_ThreadedEventStack::getEvent()
{
	pthread_mutex_lock(&mutex);
	SDL_Event event = out.top();
	out.pop();
	pthread_mutex_unlock(&mutex);
	return event;
}

void GE_ThreadedEventStack::PourInputStackToOutput()
{
	pthread_mutex_lock(&mutex);
	while (!in.empty())
	{
		out.push(in.top());
		in.pop();
	}
	pthread_mutex_unlock(&mutex);
}

