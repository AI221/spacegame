#include <pthread.h>

#include "physics.h"
#include "vector2.h"
#include "sprite.h"
#include "camera.h"

#ifndef __RENDEREDPHYSICSOBJECT_INCLUDED
#define __RENDEREDPHYSICSOBJECT_INCLUDED

enum LinkedType
{
	LINKED_NONE,
	LINKED_PHYSICS_OBJECT,
	LINKED_NETWORK_OBJECT
};

extern pthread_mutex_t RenderEngineMutex;


struct RenderedObject
{
	SDL_Renderer* renderer;
	int spriteID;
	Vector2r position; //start linked data ; we will receive our position either through networking, the physics engine, or by beaming radiation at the exact position this value is stored in RAM
	Vector2 size;
	Animation animation; //end linked data

	LinkedType linkedType;
	int linkedID;
};

extern RenderedObject* renderedObjects[1000]; //TODO dimensions and what not
extern int numRenderedObjects;

RenderedObject* GE_CreateRenderedObject(SDL_Renderer* renderer, std::string spriteName);
void GE_BlitRenderedObject(RenderedObject* subject, Camera* camera);
void GE_FreeRenderedObject(RenderedObject* subject); //will not destroy renderer,or sprite. MUST be allocated with new

#endif //__RENDEREDPHYSICSOBJECT_INCLUDED
