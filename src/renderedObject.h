/*!
 * @file
 * @author Jackson McNeill
 *
 * A simple object designed to be linked to a networkObject or physicsObject.
 */

#include <pthread.h>
#include <stack>
#include <list>

#include "vector2.h"
#include "sprite.h"
#include "camera.h"
#include "gluePhysicsObject.h"


#include "GeneralEngineCPP.h"



#ifndef __RENDEREDPHYSICSOBJECT_INCLUDED
#define __RENDEREDPHYSICSOBJECT_INCLUDED

#ifdef GE_DEBUG
#include "debugRenders.h"
#endif


#define rendered_objects_list_t std::list<GE_RenderedObject*>

/*!
 * The type of object linked to
 */
enum GE_LinkedType
{
	LINKED_NONE,
	LINKED_PHYSICS_OBJECT,
	LINKED_NETWORK_OBJECT
};

extern pthread_mutex_t RenderEngineMutex;


/*!
 * An object that has a renderer to use, a sprite to use, a position, size, and animation. 
 *
 * linkedType describes what kind of object is linked to this GE_RenderedObject, and linkedID is its ID.
 */
struct GE_RenderedObject
{
	SDL_Renderer* renderer;
	int spriteID;
	Vector2r position; //start linked data ; we will receive our position either through networking, the physics engine, or by beaming radiation at the exact position this value is stored in RAM
	Vector2 size;
	GE_Rectangle animation; //end linked data
};

extern rendered_objects_list_t renderedObjects;

/*!
 * Always call before using a rendered object
 */
int GE_RenderedObjectInit();

void GE_ChangeRenderedObjectSprite(GE_RenderedObject* subject, std::string spriteName);

/*!
 * Adds a GE_RenderedObject to the global lists of rendered objects, and returns a pointer to it back to you.
 *
 * @param renderer The renderer the GE_RenderedObject will use
 * @param spriteName The name of the spirte the GE_RenderedObject will use
 */
GE_RenderedObject* GE_CreateRenderedObject(SDL_Renderer* renderer, std::string spriteName);
GE_RenderedObject* GE_CreateRenderedObject(SDL_Renderer* renderer, std::string spriteName, int ID);

/*! 
 * Blits a rendered object and applys camera offset
 * @param subject A pointer to the GE_RenderedObject to use
 * @param camera A pointer to the camera to use
 */
void GE_BlitRenderedObject(GE_RenderedObject* subject, Camera* camera, double scale);

/*! 
 * Frees a GE_RenderedObject allocated with new. Does not destroy its renderer or sprite. 
 * @param subject A pointer to the GE_RenderedObject to free
 */
void GE_FreeRenderedObject(GE_RenderedObject* subject); 

void GE_ScheduleFreeRenderedObject(GE_RenderedObject* subject);
void GE_DeleteRenderedObjectsMarkedForDeletion();




void GE_ShutdownRenderedObject();

#endif //__RENDEREDPHYSICSOBJECT_INCLUDED
