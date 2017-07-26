/*!
 * @file
 * @author Jackson McNeill
 *
 * A simple object designed to be linked to a networkObject or physicsObject.
 */
#include <pthread.h>

#include "physics.h"
#include "vector2.h"
#include "sprite.h"
#include "camera.h"

#ifndef __RENDEREDPHYSICSOBJECT_INCLUDED
#define __RENDEREDPHYSICSOBJECT_INCLUDED

/*!
 * The type of object linked to
 */
enum LinkedType
{
	LINKED_NONE,
	LINKED_PHYSICS_OBJECT,
	LINKED_NETWORK_OBJECT
};

extern pthread_mutex_t RenderEngineMutex;


/*!
 * An object that has a renderer to use, a sprite to use, a position, size, and animation. 
 *
 * linkedType describes what kind of object is linked to this RenderedObject, and linkedID is its ID.
 */
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

/*!
 * Adds a RenderedObject to the global lists of rendered objects, and returns a pointer to it back to you.
 *
 * @param renderer The renderer the RenderedObject will use
 * @param spriteName The name of the spirte the RenderedObject will use
 */
RenderedObject* GE_CreateRenderedObject(SDL_Renderer* renderer, std::string spriteName);

/*! 
 * Blits a rendered object and applys camera offset
 * @param subject A pointer to the RenderedObject to use
 * @param camera A pointer to the camera to use
 */
void GE_BlitRenderedObject(RenderedObject* subject, Camera* camera);

/*! 
 * Frees a RenderedObject allocated with new. Does not destroy its renderer or sprite. 
 * @param subject A pointer to the RenderedObject to free
 */
void GE_FreeRenderedObject(RenderedObject* subject); 

#endif //__RENDEREDPHYSICSOBJECT_INCLUDED
