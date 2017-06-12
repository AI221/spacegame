#include "physics.h" 
//int sGrid[2000][2000]; //TODO: Dynamically sized arrays for both of these
//int collisionSpots[2000][1]; 
nextCollisionSpot = 0;
PhysicsObject* allPhysicsObjects[MAX_PHYSICS_OBJECTS];
nextPhysicsObject = 0; 
//bool deadPhysicsObjects[MAX_PHYSICS_OBJECTS];






PhysicsObject* GE_CreatePhysicsObject(Vector2r newPosition, Vector2r newVelocity, Vector2 shape)
{
	
	PhysicsObject* newPhysicsObject = new PhysicsObject{{0,0,0},{0,0,0},newPosition,true,newVelocity,true,{0,0,shape.x,shape.y},{0,0},nextPhysicsObject};
	allPhysicsObjects[nextPhysicsObject] = newPhysicsObject;
	nextPhysicsObject++;
	return newPhysicsObject;
}


/*void GE_SetPosition(Vector2r newPosition)
{
	this->newPosition = newPosition;
	setNewPosition = true;
}
void PhysicsObject::setVelocity(Vector2r newVelocity)
{
	this->newVelocity = newVelocity;
	setNewVelocity = true;
}*/
void GE_AddVelocity(PhysicsObject* physicsObject, Vector2r moreVelocity)
{
	physicsObject->newVelocity.x = physicsObject->velocity.x+moreVelocity.x;
	physicsObject->newVelocity.y = physicsObject->velocity.y+moreVelocity.y;
	physicsObject->newVelocity.r = physicsObject->velocity.r-moreVelocity.r;
}
void GE_AddRelativeVelocity(PhysicsObject* physicsObject, Vector2r moreVelocity)
{
	physicsObject->newVelocity.r = physicsObject->velocity.r-moreVelocity.r;
	physicsObject->newVelocity.addRelativeVelocity(moreVelocity.x,moreVelocity.y,physicsObject->position.r);
}
void GE_TickPhysics()
{
	for (int i=0;i < (nextPhysicsObject-1); i++)
	{
		PhysicsObject* cObj = allPhysicsObjects[i];			
		for (int x = 0; x < cObj->warpedShape.x; x++) 
		{
			for (int y = 0; y < cObj->warpedShape.y; y++) 
			{
				int posx = x+cObj->grid.x;
				int posy = y+cObj->grid.y;
				if (sGrid[posx][posy] == cObj->ID) //TODO: Check if necisary -- it might be okay to set other's grids to 0 because we'll detect collision and start an accurate simulation anyway
				{
					sGrid[posx][posy] = 0;
				}
				else if (sGrid[posx][posy] != 0)
				{
					//std::cout << "osht we gots collision im: " << myNumber << " its: " << sGrid[posx][posy] << " differenciation: " << rand() << std::endl;
				}
			}
		}
		//move ourselves forward
		//TODO: keep track of the last position, or maybe not because it could be reverse by minusing our position by our speed maybe?
		//first set velocity
		
		if (cObj->setNewVelocity) 
		{
			cObj->velocity = cObj->newVelocity;
			cObj->setNewVelocity = false;
		}
		if (cObj->setNewPosition)
		{
			cObj->position = cObj->newPosition;
			cObj->setNewPosition = false;
		}

		cObj->position += cObj->velocity;



		//TODO multiple grids, but right now we can't have you go <0 (or 2000< but that'll be fixed too and doesnt happen as much)
		if (cObj->position.x < 0)
		{
			cObj->position.x = 0;
		}
		if (cObj->position.y < 0)
		{
			cObj->position.y = 0;
		}





		cObj->grid.x = (int) cObj->position.x/10;
		cObj->grid.y = (int) cObj->position.y/10;

		//Calculate how the shape warps. The shape will stretch as you move faster to avoid clipping
		cObj->warpedShape.x = ((abs(cObj->velocity.x)/10)+2)*(cObj->grid.shapex/10);
		cObj->warpedShape.y = ((abs(cObj->velocity.y)/10)+2)*(cObj->grid.shapey/10);

		for (int x = 0; x < cObj->warpedShape.x; x++) 
		{
			for (int y = 0; y < cObj->warpedShape.y; y++) 
			{
				int posx = x+cObj->grid.x;
				int posy = y+cObj->grid.y;
				if (sGrid[posx][posy] == 0) 
				{
					sGrid[posx][posy] = cObj->ID;
				}
				else
				{
					collisionSpots[nextCollisionSpot][0] = posx;
					collisionSpots[nextCollisionSpot][1] = posy;
					nextCollisionSpot++;
				}
			}
		}
	}


}
void GE_FreePhysicsObject(PhysicsObject* physicsObject) //MUST be allocated with new
{
	deadPhysicsObjects[physicsObject->ID] = true;
	delete physicsObject;
}
