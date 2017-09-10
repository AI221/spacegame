#include "network.h"

#ifdef NETWORKING_ENABLED

#ifndef __NETWORKOBJECT_INCLUDED
#define __NETWORKOBJECT_INCLUDED



enum GE_NETWORKING_INSTRUCTION : char
{
	GE_NETWORKING_INSTRUCTION_INCOMING_INSTURCTION_BYTE,
	GE_NETWORKING_INSTRUCTION_END_TICK_TRANSMISSION,
	GE_NETWORKING_INSTRUCTION_END_INSTRUCTION,
	GE_NETWORKING_INSTRUCTION_NEW_RENDERED_OBJECT, //int ID, 
	GE_NETWORKING_INSTRUCTION_DELETE_RENDERED_OBJECT, //int ID
	GE_NETWORKING_INSTRUCTION_UPDATE_RENDERED_OBJECT, //
};



#endif // __NETWORKOBJECT_INCLUDED

#endif //NETWORKING_ENABLED
