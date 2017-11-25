/*!
 * @file
 * @author Jackson McNeill
 *
 * Allows the game engine to have a better custom event system than SDL events. Also wraps arround SDL events
 */
#include <SDL2/SDL.h>

#ifndef __EVENT_DEFINED
#define __EVENT_DEFINED

/*!
 * Initialize events subsystem
 */
void GE_InitEvents();

/*!
 * Get a new ID for an event. Undefined behaviour if you've registered more events than 2^64.
 */
unsigned long long int GE_Event_RegisterEvent();

/*!
 * An event class to be inherited from. Undefined behaviour if you assign type to a value not assigned by GE_Event_RegisterEvent()
 */
class GE_Event
{
	public:	
		virtual ~GE_Event() {}
		unsigned long long int type;
};


extern unsigned long long int GE_EventType_SDLEvent;
class GE_SDLEvent : public GE_Event
{
	public:
		GE_SDLEvent(SDL_Event event);
		~GE_SDLEvent();

		SDL_Event event;
};



#endif //__EVENT_DEFINED
