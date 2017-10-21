#include "event.h"


unsigned long long int numEventsRegistered = 0;
unsigned long long int GE_EventType_SDLEvent;

unsigned long long int GE_Event_RegisterEvent() //if you manage to find a real world case of hitting the event limit I will add error codes to this for you free of charge.
{
	numEventsRegistered++; //intentionally leave out event #0 just in case we wanted an error code
	return numEventsRegistered;
}



void GE_InitEvents()
{
	GE_EventType_SDLEvent = GE_Event_RegisterEvent();
}
GE_SDLEvent::GE_SDLEvent(SDL_Event event)
{
	this->event = event;

	this->type = GE_EventType_SDLEvent;
}
GE_SDLEvent::~GE_SDLEvent()
{
	 //do nothing
}
