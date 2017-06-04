#include <math.h>
#include <stdlib.h>
//Local includes
#include "vector2.h"

#ifndef __PHYSICS_INCLUDED
#define __PHYSICS_INCLUDED

#define MAX_PHYSICS_OBJECTS 256000000 //maximum ammount of physics objects in the game
struct GridInfo //info for the inaccurate physics pre-calculation
{
	int x;
	int y;
	int shapex;
	int shapey; 
};

extern int sGrid[2000][2000]; //TODO: Dynamically sized arrays for both of these

extern int collisionSpots[2000][1]; 
extern int nextCollisionSpot;

struct PhysicsObject
{
	struct Vector2r position;
	struct Vector2r velocity;
	struct Vector2r newPosition;
	bool setNewPosition;
	struct Vector2r newVelocity;
	bool setNewVelocity;
	struct GridInfo grid;
	struct Vector2 warpedShape;
	int ID;
};



extern PhysicsObject* allPhysicsObjects[MAX_PHYSICS_OBJECTS];
extern int nextPhysicsObject; 

extern bool deadPhysicsObjects[MAX_PHYSICS_OBJECTS];


PhysicsObject* GE_CreatePhysicsObject(Vector2r newPosition, Vector2r newVelocity, Vector2 shape);
void GE_AddVelocity(PhysicsObject* physicsObject, Vector2r moreVelocity);
void GE_AddRelativeVelocity(PhysicsObject* physicsObject, Vector2r moreVelocity);
void GE_TickPhysics();
void GE_FreePhysicsObject(PhysicsObject* physicsObject); //MUST be allocated with new

//probably will change this to a dynamically sized array later

#endif 
