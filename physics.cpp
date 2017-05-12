#include "physics.h"

PhysicsObject::PhysicsObject(Vector2r newPosition, Vector2 shape)
{
	allPhysicsObjects[nextPhysicsObject] = this;
	myNumber = nextPhysicsObject;
	nextPhysicsObject++;

	position.x = newPosition.x;
	position.y = newPosition.y;
	velocity = {0,0};
	grid = {0,0,0,0};
	warpedShape = {0,0};

	

	//setPosition(newPosition); //commented this out b/c it waits till next physics tick to update position
	
	setShape(shape);
}
void PhysicsObject::setPosition(Vector2r newPosition)
{
	this->newPosition = newPosition;
	setNewPosition = true;
}
void PhysicsObject::setVelocity(Vector2r newVelocity)
{
	this->newVelocity = newVelocity;
	setNewVelocity = true;
}
void PhysicsObject::addVelocity(Vector2r moreVelocity)
{
	newVelocity.x = velocity.x+moreVelocity.x;
	newVelocity.y = velocity.y+moreVelocity.y;
	newVelocity.r = velocity.r-moreVelocity.r;
	setNewVelocity = true;
}
void PhysicsObject::addRelativeVelocity(Vector2r moreVelocity)
{
	newVelocity.r = velocity.r-moreVelocity.r;
	newVelocity.addRelativeVelocity(moreVelocity.x,moreVelocity.y,this->position.r);
	setNewVelocity = true;
}
Vector2r PhysicsObject::getVelocity()
{
	return velocity;
}
Vector2r PhysicsObject::getPosition()
{
	return position;
}

void PhysicsObject::setShape(Vector2 shape)
{
	grid.shapex = shape.x;
	grid.shapey = shape.y;
}
void PhysicsObject::tickMyPhysics() //the fun part
{
	for (int x = 0; x < warpedShape.x; x++) 
	{
		for (int y = 0; y < warpedShape.y; y++) 
		{
			int posx = x+grid.x;
			int posy = y+grid.y;
			if (sGrid[posx][posy] == myNumber) //TODO: Check if necisary -- it might be okay to set other's grids to 0 because we'll detect collision and start an accurate simulation anyway
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
	
	if (setNewVelocity) 
	{
		velocity = newVelocity;
		setNewVelocity = false;
	}
	if (setNewPosition)
	{
		position = newPosition;
		setNewPosition = false;
	}

	position.x += velocity.x;
	position.y += velocity.y;
	position.r += velocity.r;

	//prevent rotation from being >360
	position.r -= floor(position.r/360)*360;


	//TODO multiple grids, but right now we can't have you go <0 (or 2000< but that'll be fixed too and doesnt happen as much)
	if (position.x < 0)
	{
		position.x = 0;
	}
	if (position.y < 0)
	{
		position.y = 0;
	}





	grid.x = (int) position.x/10;
	grid.y = (int) position.y/10;

	//Calculate how the shape warps. The shape will stretch as you move faster to avoid clipping
	warpedShape.x = ((abs(velocity.x)/10)+2)*(grid.shapex/10);
	warpedShape.y = ((abs(velocity.y)/10)+2)*(grid.shapey/10);

	for (int x = 0; x < warpedShape.x; x++) 
	{
		for (int y = 0; y < warpedShape.y; y++) 
		{
			int posx = x+grid.x;
			int posy = y+grid.y;
			if (sGrid[posx][posy] == 0) 
			{
				sGrid[posx][posy] = myNumber;
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


