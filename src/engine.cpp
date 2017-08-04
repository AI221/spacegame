#include "engine.h"

int GE_Init(SDL_Renderer* renderer)
{
	int error;
	error = GE_SpriteInit(renderer);
	if (error != 0)
	{
		return error;
	}
	error = GE_PhysicsInit();
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
