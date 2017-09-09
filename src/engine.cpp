#include "engine.h"

int GE_Init(SDL_Renderer* renderer)
{
	GE_IsOn = true;
	int error;
	error = GE_SpriteInit(renderer);
	if (error != 0)
	{
		return error;
	}
#ifndef PHYSICS_DEBUG_SLOWRENDERS
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

void GE_Shutdown()
{
	GE_IsOn = false;
	GE_ShutdownPhysicsEngine();
	GE_FreeAllSprites();

}
