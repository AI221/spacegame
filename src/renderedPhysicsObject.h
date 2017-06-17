#include "physics.h"
#include "vector2.h"
#include "sprite.h"
#include "camera.h"

#ifndef __RENDEREDPHYSICSOBJECT_INCLUDED
#define __RENDEREDPHYSICSOBJECT_INCLUDED

struct RenderedPhysicsObject
{
	SDL_Renderer* renderer;
	Sprite* sprite;
	PhysicsObject* physicsObject;
};

extern RenderedPhysicsObject* physicsObjects[1000]; //TODO dimensions and what not
extern int numPhysicsObjs; //Starts at 1 because sGrid uses 0 for 'nothing here'

RenderedPhysicsObject* GE_CreateRenderedPhysicsObject(SDL_Renderer* renderer, Sprite* sprite,Vector2r newPosition, Vector2r newVelocity, Vector2 shape);
void GE_BlitRenderedPhysicsObject(RenderedPhysicsObject* subject, Camera* camera);
void GE_FreeRenderedPhysicsObject(RenderedPhysicsObject* renderedPhysicsObject); //will not destroy renderer,or sprite. MUST be allocated with new
#endif //__RENDEREDPHYSICSOBJECT_INCLUDED

