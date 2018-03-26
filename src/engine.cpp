#include "engine.h"

#include <SDL2/SDL.h>
#include <time.h>

#include "gluePhysicsObject.h"
#include "gluePhysicsObjectInit.h"
#include "physics.h"
#include "sprite.h"
#include "renderedObject.h"
#include "isOn.h"
#include "json.h"
#include "UI.h"
#include "font.h"
#include "serializeObject.h"

//Static asserts
#include  <limits>


//we need this for portability in serialization
static_assert(std::numeric_limits<double>::is_iec559,"Doubles must be implemented as IEEE 754!\n");

int GE_Init(SDL_Renderer* renderer)
{
	GE_IsOn.store(true);
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
	error = GE_Font_Init();
	if (error != 0)
	{
		return error+500;
	}
	error  = GE_Init_SerializeObject();
	if (error != 0)
	{
		return error+600;
	}
	
	
	srand(time(NULL)); //set random seed to time

	return 0;
}

void GE_Shutdown()
{
	printf("----FULL ENGINE SHUTDOWN----\n");
	GE_IsOn.store(false);
	while (!PhysicsEngineThreadShutdown.load()) //dont want to free physics engine things before it's finished a tick. 
	{
		SDL_Delay(16);
#ifdef PHYSICS_DEBUG_SLOWRENDERS 
		//no physics thread, break to avoid hault
		break;
#endif
	} 
	GE_ShutdownPhysicsEngine();
	printf("sprites\n");
	GE_FreeAllSprites();
	printf("UI\n");
	GE_ShutdownUI();
	printf("RenderedObject\n");
	GE_ShutdownRenderedObject();
	printf("Font\n");
	GE_Font_Shutdown();
	printf("Json\n");
	GE_JsonShutdown();


}
