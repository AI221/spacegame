#include "physics.h"


PhysicsObject* allPhysicsObjects[MAX_PHYSICS_OBJECTS];
int nextPhysicsObject = 1; 
bool deadPhysicsObjects[MAX_PHYSICS_OBJECTS];

int sGrid[2000][2000]; //TODO: Dynamically sized arrays for both of these
//int collisionSpots[2000][1]; 
//int nextCollisionSpot = 0;

PhysicsObject* GE_CreatePhysicsObject(Vector2r newPosition, Vector2r newVelocity, Vector2 shape)
{
	
	PhysicsObject* newPhysicsObject = new PhysicsObject{{0,0,0},{0,0,0},newPosition,true,newVelocity,true,{0,0,shape.x,shape.y},{0,0},nextPhysicsObject,{},0};
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
	physicsObject->setNewVelocity = true;
}
void GE_AddRelativeVelocity(PhysicsObject* physicsObject, Vector2r moreVelocity)
{
	physicsObject->newVelocity = physicsObject->velocity;
	physicsObject->newVelocity.r = physicsObject->velocity.r-moreVelocity.r;
	physicsObject->newVelocity.addRelativeVelocity(moreVelocity.x,moreVelocity.y,physicsObject->position.r);
	physicsObject->setNewVelocity = true;
}
int numCollisionsTemp = 0;
void GE_TickPhysics()
{
	
	for (int i=1;i < (nextPhysicsObject-1); i++)
	{
		PhysicsObject* cObj = allPhysicsObjects[i];			
		
		//remove our old grid 
		for (int x = 0; x < cObj->warpedShape.x; x++) 
		{
			for (int y = 0; y < cObj->warpedShape.y; y++) 
			{
				int posx = x+cObj->grid.x;
				int posy = y+cObj->grid.y;
				if (sGrid[posx][posy] == cObj->ID) //do not remove other's sGrid entries. if we did, we wouldn't ever detect collision.
				{
					sGrid[posx][posy] = 0;
				}
			}
		}
		//move ourselves forward
		//first set velocity
		
		if (cObj->setNewVelocity) 
		{
			cObj->velocity = cObj->newVelocity;
			cObj->newVelocity = {0,0,0};
			cObj->setNewVelocity = false;
		}
		if (cObj->setNewPosition)
		{
			cObj->position = cObj->newPosition;
			cObj->newPosition = {0,0,0};
			cObj->setNewPosition = false;
		}

		cObj->position = cObj->position+cObj->velocity;



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

		/*for (int x = 0; x < cObj->warpedShape.x; x++) 
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
		}*/

	
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
				if (sGrid[posx][posy] != 0 && sGrid[posx][posy] != cObj->ID)
				{
					//collision full-check
					
					std::cout << "osht we gots collision im: " << cObj->ID << " its: " << sGrid[posx][posy] << " differenciation: " << rand() << std::endl;
					std::cout << " im x " << cObj->position.x << " y " << cObj->position.y << std::endl;
				
					PhysicsObject* victimObj = allPhysicsObjects[sGrid[posx][posy]];

					std::cout << "my rect count " << cObj->numCollisionRectangles << std::endl;

					std::cout << "their rect count " << victimObj->numCollisionRectangles << std::endl;

					for (int a=0; a < cObj->numCollisionRectangles;a++)
					{
						//iterate through each of our rectangles...

						//The left two points are simply the location of the rectangle relative to the location of cObj. The right 2 points are the location of the rectangle relative to the location of cObj plus the width or height of the rectangle.
						//TODO: incorportate rotation

						Rectangle currentRectangle = cObj->collisionRectangles[a];
						//double myPoints[4] = {currentRectangle.x+cObj->position.x,currentRectangle.x+currentRectangle.w+cObj->position.x,currentRectangle.y+cObj->position.y,currentRectangle.y+currentRectangle.h+cObj->position.y};

						Vector2 myPoints[4] = {
							{currentRectangle.x+cObj->position.x , currentRectangle.y+cObj->position.y}, //top left
							{currentRectangle.x+currentRectangle.w+cObj->position.x , currentRectangle.y+cObj->position.y}, //top right
							{currentRectangle.x+cObj->position.x , currentRectangle.y+currentRectangle.h+cObj->position.y}, //bottom left
							{currentRectangle.x+currentRectangle.w+cObj->position.x , currentRectangle.y+currentRectangle.h+cObj->position.y} //bottom right
						};



						for (int b=0; b < victimObj->numCollisionRectangles;b++)
						{

							Rectangle victimRectangle = victimObj->collisionRectangles[b];
							Vector2 theirPoints[4] = {
								{victimRectangle.x+victimObj->position.x , victimRectangle.y+victimObj->position.y}, //top left
								{victimRectangle.x+victimRectangle.w+victimObj->position.x , victimRectangle.y+victimObj->position.y}, //top right
								{victimRectangle.x+victimObj->position.x , victimRectangle.y+victimRectangle.h+victimObj->position.y}, //bottom left
								{victimRectangle.x+victimRectangle.w+victimObj->position.x , victimRectangle.y+victimRectangle.h+victimObj->position.y} //bottom right
							};
							

							//iterate through each of my points, checking it against each victim point.
							for (int me = 0;me < 4; me++)
							{
								std::cout << me << std::endl;
								std:: cout << " x " << myPoints[me].x << " y " << myPoints[me].y  << std::endl;
								
								bool check1 = (myPoints[me].x > theirPoints[0].x) && (myPoints[me].y > theirPoints[0].y);
								bool check2 = true;//(myPoints[me].x > theirPoints[0].
								bool check3 = true;
								bool check4 = (myPoints[me].x < theirPoints[3].x) && (myPoints[me].y < theirPoints[3].y);

								std::cout << "c1 " << check1 << " c4 " << check4 << std::endl;

								if (check1 && check2 && check3 && check4)
								{
									numCollisionsTemp++;
									std::cout << "FULL COLLISION DETECTED #" << numCollisionsTemp << std::endl;

									Vector2r newVelocity;

									newVelocity.x = (cObj->velocity.x*.5)+(victimObj->velocity.x*.5);
									newVelocity.y = (cObj->velocity.y*.5)+(victimObj->velocity.y*.5);
									newVelocity.r = (cObj->velocity.r*.5)+(victimObj->velocity.r*.5);


									/*cObj->velocity.x = (cObj->velocity.x*.5)+(victimObj->velocity.x*(-0.5));
									cObj->velocity.y = (cObj->velocity.y*.5)+(victimObj->velocity.y*(-0.5));
									cObj->velocity.r = (cObj->velocity.r*.5)+(victimObj->velocity.r*(-0.5));


									victimObj->velocity.x = (victimObj->velocity.x*(-0.5))+(oldVelocity.x*(0.5));
									victimObj->velocity.y = (victimObj->velocity.y*(-0.5))+(oldVelocity.y*(0.5));
									victimObj->velocity.r = (victimObj->velocity.r*(-0.5))+(oldVelocity.r*(0.5));*/

									cObj->velocity.x = newVelocity.x*-1;
									cObj->velocity.y = newVelocity.y*-1;
									cObj->velocity.r = newVelocity.r*-1;
									cObj->velocity = newVelocity;
									victimObj->velocity = newVelocity;

								}
								for (int victim = 0;victim <4; victim++)
								{
									//jesus christ this is a lot of for loops. i feel sorry for the sucker who has to debug this
									//...oh...

									/*if ((myPoints[me].x > theirPoints[victim].x) && (myPoints[me].y > theirPoints[victim].y))
									{
										std::cout << "FULL COLLISION DETECTED" << std::endl;
									}*/
									

								}
							}
						}
					}

				}
			}
		}

	}


}
void GE_FreePhysicsObject(PhysicsObject* physicsObject) //MUST be allocated with new
{
	deadPhysicsObjects[physicsObject->ID] = true;
	delete physicsObject->collisionRectangles;
	delete physicsObject;
}


