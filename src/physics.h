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


struct GridInfo //info for the inaccurate physics pre-calculation
{
	int x;
	int y;
	int shapex;
	int shapey; 
};


/*
	x=0;y=0;w=10;h=10;

	   width
	x,y-----|
	|		|
	|		|
  h |		|
  e |		|
  i |		| 
  g |		|
  h |		|
  t |		|
	|-------|<-- 10,10 (0+10,0+10)

*/

struct Rectangle //TODO allow rectangles themselves to be rotated relative to their owners
{
	double x;
	double y;
	double w;
	double h;
};

struct PhysicsObject
{
	Vector2r position;
	Vector2r velocity;
	Vector2r newPosition;
	bool setNewPosition;
	Vector2r newVelocity;
	bool setNewVelocity;
	GridInfo grid;
	Vector2 warpedShape;
	int ID;
	Rectangle collisionRectangles[MAX_COLLISION_RECTANGLES_PER_OBJECT];
	int numCollisionRectangles;
	Vector2r lastGoodPosition;
	bool callCallbackBeforeCollisionFunction;
	bool callCallbackAfterCollisionFunction;
	std::function< bool (PhysicsObject* cObj, PhysicsObject* victimObj)> C_Collision; //if return true, skip this physicsObject ( useful if you delete yourself)
};


extern pthread_t PhysicsEngineThread;

extern pthread_mutex_t PhysicsEngineMutex;

	//pthread_create(&GlueThread,NULL,GE_glueThreadMain,NULL);

extern PhysicsObject* physicsObjects[MAX_PHYSICS_OBJECTS];
extern int numPhysicsObjects; 

extern int fakeToRealPhysicsID[MAX_PHYSICS_OBJECTS]; 
extern int numFakePhysicsIDs;

extern std::function< void ()> C_PhysicsTickDoneCallbacks[MAX_PHYSICS_ENGINE_DONE_CALLBACKS];
extern int numPhysicsTickDoneCallbacks;

extern bool deadPhysicsObjects[MAX_PHYSICS_OBJECTS];


extern int sGrid[2000][2000]; //TODO: Dynamically sized arrays for both of these

//extern int collisionSpots[2000][1]; 
//extern int nextCollisionSpot;




int GE_PhysicsInit();

PhysicsObject* GE_CreatePhysicsObject(Vector2r newPosition, Vector2r newVelocity, Vector2 shape);
int GE_GetPhysicsObjectFromID(int fakeID, PhysicsObject** physicsObjectPointer);

void GE_AddPhysicsDoneCallback(std::function<void ()> callback);
void GE_AddPhysicsObjectCollisionCallback(PhysicsObject* subject, std::function< bool (PhysicsObject* cObj, PhysicsObject* victimObj)> C_Collision, bool callCallbackBeforeCollisionFunction);

void GE_AddVelocity(PhysicsObject* physicsObject, Vector2r moreVelocity);
void GE_AddRelativeVelocity(PhysicsObject* physicsObject, Vector2r moreVelocity);

void* GE_physicsThreadMain(void* ); 
void GE_TickPhysics();
void GE_TickPhysics_ForObject(PhysicsObject* cObj);
void GE_CollisionFullCheck(PhysicsObject* cObj, PhysicsObject* victimObj);

void GE_FreePhysicsObject(PhysicsObject* physicsObject); //MUST be allocated with new

void GE_RectangleToPoints(Rectangle rect, Vector2* points, Vector2r hostPosition);

#endif //PHYSICS_INCLUDED
