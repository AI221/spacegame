#include "renderedPhysicsObject.h"

RenderedPhysicsObject* physicsObjects[1000]; //TODO dimensions and what not
int numPhysicsObjs = 1; //Starts at 1 because sGrid uses 0 for 'nothing here'

RenderedPhysicsObject* GE_CreateRenderedPhysicsObject(SDL_Renderer* renderer, Sprite* sprite,Vector2r newPosition, Vector2r newVelocity, Vector2 shape)
{
	PhysicsObject* physicsObject = GE_CreatePhysicsObject(newPosition, newVelocity, shape);
	RenderedPhysicsObject* renderedPhysicsObject = new RenderedPhysicsObject{renderer, sprite, physicsObject };
	return renderedPhysicsObject;
}
void GE_BlitRenderedPhysicsObject(RenderedPhysicsObject* subject, Camera* camera)
{
	Vector2r position = GE_ApplyCameraOffset(camera,subject->physicsObject->position);
	GE_BlitSprite(subject->sprite,position,{0,0}); //TODO

}
void GE_FreeRenderedPhysicsObject(RenderedPhysicsObject* renderedPhysicsObject) //will not destroy renderer,or sprite. MUST be allocated with new
{
	GE_FreePhysicsObject(renderedPhysicsObject->physicsObject);
	delete renderedPhysicsObject;
}


