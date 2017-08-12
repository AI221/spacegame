#include "engine.h"

int GE_Init(SDL_Renderer* renderer)
{
	int error;
	error = GE_SpriteInit(renderer);
	if (error != 0)
	{
		return error;
	}
#ifndef physics_debug
	error = GE_PhysicsInit();
#endif
	if (error != 0)
	{
		return error+10;
	}
	error = GE_GlueInit();
	if (error != 0)
	{
		return error+100;
	}

	return 0;
}
