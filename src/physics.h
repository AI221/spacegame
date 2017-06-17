#include <math.h>
#include "vector2.h"

#ifndef __PHYSICS_INCLUDED
#define __PHYSICS_INCLUDED

#define MAX_PHYSICS_OBJECTS 256//256000000 //maximum ammount of physics objects in the game
struct GridInfo //info for the inaccurate physics pre-calculation
{
	int x;
	int y;
	int shapex;
	int shapey; 
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
};

extern PhysicsObject* allPhysicsObjects[MAX_PHYSICS_OBJECTS];
extern int nextPhysicsObject; 

extern bool deadPhysicsObjects[MAX_PHYSICS_OBJECTS];


extern int sGrid[2000][2000]; //TODO: Dynamically sized arrays for both of these

extern int collisionSpots[2000][1]; 
extern int nextCollisionSpot;




PhysicsObject* GE_CreatePhysicsObject(Vector2r newPosition, Vector2r newVelocity, Vector2 shape);
void GE_AddVelocity(PhysicsObject* physicsObject, Vector2r moreVelocity);
void GE_AddRelativeVelocity(PhysicsObject* physicsObject, Vector2r moreVelocity);
void GE_TickPhysics();
void GE_FreePhysicsObject(PhysicsObject* physicsObject); //MUST be allocated with new



#endif //PHYSICS_INCLUDED
