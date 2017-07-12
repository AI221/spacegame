#include "physics.h"
#include "vector2.h"
#include "sprite.h"
#include "camera.h"

#ifndef __RENDEREDPHYSICSOBJECT_INCLUDED
#define __RENDEREDPHYSICSOBJECT_INCLUDED

struct RenderedObject
{
	SDL_Renderer* renderer;
	int spriteID;
	Vector2r position; //we will receive our position either through networking, the physics engine, or through random generation if you want i guess
};

extern RenderedObject* renderedObjects[1000]; //TODO dimensions and what not
extern int countRenderedObjects;

RenderedObject* GE_CreateRenderedObject(SDL_Renderer* renderer, std::string spriteName);
void GE_BlitRenderedObject(RenderedObject* subject, Camera* camera);
void GE_FreeRenderedObject(RenderedObject* subject); //will not destroy renderer,or sprite. MUST be allocated with new

#endif //__RENDEREDPHYSICSOBJECT_INCLUDED
