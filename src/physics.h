/*!
 *@file
 *@author Jackson McNeill
 *
 * An overkill 2D physics engine. Before checking "full" collision, there is a preliminatory check which involves sGrid.
 *
 * This preliminatory check works by setting elements in the 2D sGrid array to their physicsID. When overlap in this is detected, a full collision check happens. TODO: better explanation 
 */


#include <pthread.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>

#include <iostream>
#include <functional>

#include "vector2.h"

//#define physics_debug 

#include<SDL2/SDL.h> //TODO temp
#ifdef physics_debug
	//#include<SDL2/SDL.h>
	#include "camera.h"
	void GE_DEBUG_PassRendererToPhysicsEngine(SDL_Renderer* yourRenderer, Camera* yourCamera);
#endif

#ifndef __PHYSICS_INCLUDED
#define __PHYSICS_INCLUDED

//LIMITS:
#define MAX_PHYSICS_OBJECTS 1024 //maximum ammount of physics objects in the game
#define MAX_COLLISION_RECTANGLES_PER_OBJECT 32 //TODO: test if this is a good limit in practice
#define MAX_PHYSICS_ENGINE_DONE_CALLBACKS 64



struct GE_PhysicsObject
{
	Vector2r position;
	Vector2r velocity;
	Vector2r newPosition;
	bool setNewPosition;
	Vector2r newVelocity;
	bool setNewVelocity;
	GE_Rectangle grid;
	Vector2 warpedShape;
	int ID;
	GE_Rectangle collisionRectangles[MAX_COLLISION_RECTANGLES_PER_OBJECT];
	int numCollisionRectangles;
	Vector2r lastGoodPosition;
	bool callCallbackBeforeCollisionFunction;
	bool callCallbackAfterCollisionFunction;
	std::function< bool (GE_PhysicsObject* cObj, GE_PhysicsObject* victimObj)> C_Collision; //if return true, skip this physicsObject ( useful if you delete yourself)
};


extern pthread_t PhysicsEngineThread;

extern pthread_mutex_t PhysicsEngineMutex;

	//pthread_create(&GlueThread,NULL,GE_glueThreadMain,NULL);

extern GE_PhysicsObject* physicsObjects[MAX_PHYSICS_OBJECTS];
extern int numPhysicsObjects; 

extern int fakeToRealPhysicsID[MAX_PHYSICS_OBJECTS]; 
extern int numFakePhysicsIDs;

extern std::function< void ()> C_PhysicsTickDoneCallbacks[MAX_PHYSICS_ENGINE_DONE_CALLBACKS];
extern int numPhysicsTickDoneCallbacks;

extern bool deadPhysicsObjects[MAX_PHYSICS_OBJECTS];


extern int sGrid[2000][2000]; //TODO: Dynamically sized arrays for both of these

//extern int collisionSpots[2000][1]; 
//extern int nextCollisionSpot;



/*!
 * Spawns a new thread containing the physics engine. 
*/
int GE_PhysicsInit();


GE_PhysicsObject* GE_CreatePhysicsObject(Vector2r newPosition, Vector2r newVelocity, Vector2 shape);


/*!
 * Success: Gives you the physics object an ID is pointing to
 *
 * Failure: Doesn't touch your pointer, returns error code>0
 * @return 0: Success 1: Object is dead or otherwise doesn't exist.
 * @param fakeID The ID of the physics object 
 * @param physicsObjectPointer - A pointer to a pointer of a physics object. This will modify the pointer of a physics object.
 */
int GE_GetPhysicsObjectFromID(int fakeID, GE_PhysicsObject** physicsObjectPointer);

/*!
 * Adds a callback which will be ran after each physics tick. Used by the engine itself for gluing things to the physics engine.
 *
 * @param callback A function that gives void output and takes no input
 */
void GE_AddPhysicsDoneCallback(std::function<void ()> callback);

/*!
 * Adds a callback to an individual physics object that will be called when it collides with any other physics object. It is not gauranteed your callback will be called first, or at all if your physics object is killed.
 * @param subject The physics object to add the callback to
 * @param C_Collision A function that takes GE_PhysicsObject* 's cObj(you) and victimObj(your collision partner) and  outputs true when a either cObj or victimObj has been killed. 
 * @param callCallbackBeforeCollisionFunction True to call the callback BEFORE velocities have been exchanged, false to call it AFTER. 
 */
void GE_AddPhysicsObjectCollisionCallback(GE_PhysicsObject* subject, std::function< bool (GE_PhysicsObject* cObj, GE_PhysicsObject* victimObj)> C_Collision, bool callCallbackBeforeCollisionFunction);

/*!
 * Add velocity to a physics object. Note that only 1 additional velocity will be added per tick, so calling it 3 times will result in the call being added.
 *
 * @param physicsObject The physics object to add the velocity to
 * @param moreVelocity How much more velocity to add
 */
void GE_AddVelocity(GE_PhysicsObject* physicsObject, Vector2r moreVelocity);

/*! 
 * Add velocity to the physics object, relative to its rotation. 
 *
 * @param physicsObject The physics object to add the velocity to
 * @param moreVelocity How much more velocity to add
 */
void GE_AddRelativeVelocity(GE_PhysicsObject* physicsObject, Vector2r moreVelocity);

/*! 
 * The function made into a new thread by GE_PhysicsInit . In general: Don't touch this
 */
void* GE_physicsThreadMain(void* ); 

/*! 
 * The function called every tick. In general: Don't touch this
 */
void GE_TickPhysics();

/*!
 * The function called for every physics object, during a physics tick. In general: Don't touch this
 * @param cObj The pointer to the physics object to tick
 */ 
void GE_TickPhysics_ForObject(GE_PhysicsObject* cObj);

/*!
 * The function called when a full collision check is determined to be necessary. In general: Don't touch this
 */
void GE_CollisionFullCheck(GE_PhysicsObject* cObj, GE_PhysicsObject* victimObj);

/*! 
 * Frees the memory used by a physics object. 
 * @param physicsObject The physics object which will be DELETED and you can no longer use afterwards
 */
void GE_FreePhysicsObject(GE_PhysicsObject* physicsObject); //MUST be allocated with new

/*!
 * Converts a rectangle to 4 points in 2D space. A point in 2D space has no rotation, so it is described by a Vector2. Use it if you want.
 * @param rect The rectangle to be converted
 * @param points An array that can contain 4 Vector2* s. 
 * @param hostPosition a Vector2r , note the r, that the GE_Rectangle rect belongs to. It is used to be added to the positions of points, and its rotation will translate them.
 */
void GE_RectangleToPoints(GE_Rectangle rect, Vector2* points, Vector2r hostPosition);

#endif //PHYSICS_INCLUDED
