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
		return error+100;
	}
	error = GE_GlueInit();
	if (error != 0)
	{
		return error+200;
	}
	error = GE_RenderedObjectInit();
	if (error != 0)
	{
		return error+300;
	}
	error = GE_JsonInit();
	if (error != 0)
	{
		return error+400;
	}
	
	
	srand(time(NULL)); //set random seed to time

	return 0;
}

void GE_Shutdown()
{
	printf("----FULL ENGINE SHUTDOWN----\n");
	GE_IsOn = false;
	GE_ShutdownPhysicsEngine();
	printf("sprits\n");
	GE_FreeAllSprites();

}
