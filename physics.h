#include "vector2.h"
#include <math.h>
#include <stdlib.h>


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

int sGrid[2000][2000]; //TODO: Dynamically sized arrays for both of these

int collisionSpots[2000][1]; 
int nextCollisionSpot = 0;

class PhysicsObject
{
	public:
		Vector2r getPosition();
		Vector2r getVelocity();
		GridInfo getGridInfo();
		PhysicsObject(Vector2r newPosition, Vector2 shape);

		void setPosition(Vector2r newPosition);
		void setVelocity(Vector2r newVelocity);
		void addVelocity(Vector2r moreVelocity);
		void addRelativeVelocity(Vector2r moreVelocity);

		void setShape(Vector2 shape);

		void tickMyPhysics(); //important: this should NOT have collision detection. that would result on instances where things would collide even though the object they collide with would've moved next tick
		Vector2r position;
		Vector2r velocity; 

	private:

		Vector2r newPosition;
		bool setNewPosition = false;

		Vector2r newVelocity;
		bool setNewVelocity = false;

		GridInfo grid;
		Vector2 warpedShape;

		int myNumber;


};



PhysicsObject* allPhysicsObjects[MAX_PHYSICS_OBJECTS];
int nextPhysicsObject = 1; //TODO: Merge with the physicsObjects
//probably will change this to a dynamically sized array later

#endif 
