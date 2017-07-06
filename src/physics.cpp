#include "physics.h"




PhysicsObject* allPhysicsObjects[MAX_PHYSICS_OBJECTS];
int nextPhysicsObject = 1; 
bool deadPhysicsObjects[MAX_PHYSICS_OBJECTS]; //TODO shift all elements down instead of this patch

int sGrid[2000][2000]; //TODO: Dynamically sized arrays for both of these
//int collisionSpots[2000][1]; 
//int nextCollisionSpot = 0;

PhysicsObject* GE_CreatePhysicsObject(Vector2r newPosition, Vector2r newVelocity, Vector2 shape)
{
	
	PhysicsObject* newPhysicsObject = new PhysicsObject{{0,0,0},{0,0,0},newPosition,true,newVelocity,true,{0,0,shape.x,shape.y},{0,0},nextPhysicsObject,{},0,{0,0,0},false};
	allPhysicsObjects[nextPhysicsObject] = newPhysicsObject;
	nextPhysicsObject++;
	return newPhysicsObject;
}
void GE_AddPhysicsObjectCollisionCallback(PhysicsObject* subject, std::function< bool (PhysicsObject* cObj, PhysicsObject* victimObj)> C_Collision, bool callCallbackBeforeCollisionFunction)
{
	subject->C_Collision = C_Collision;
	subject->callCallbackBeforeCollisionFunction = callCallbackBeforeCollisionFunction;
	subject->callCallbackAfterCollisionFunction = !callCallbackBeforeCollisionFunction;
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
	physicsObject->newVelocity.addRelativeVelocity({moreVelocity.x,moreVelocity.y,physicsObject->position.r}); //TODO: De-OOify
	physicsObject->setNewVelocity = true;
}
int numCollisionsTemp = 0;
void GE_TickPhysics()
{
	for (int i=1;i < (nextPhysicsObject); i++)
	{
		GE_TickPhysics_ForObject(allPhysicsObjects[i]);
	}
}
void GE_TickPhysics_ForObject(PhysicsObject* cObj)
{
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



	//TODO multiple grids, but right now we can't have you go <0 
	if (cObj->position.x < 0)
	{
		cObj->position.x = 0;
		cObj->velocity.x = 0;
	}
	if (cObj->position.y < 0)
	{
		cObj->position.y = 0;
		cObj->velocity.y = 0;
	}
	//..or >2000
	if (cObj->position.x > 2000)
	{
		cObj->position.x = 2000;
		cObj->velocity.x = 0;
	}
	if (cObj->position.y > 2000)
	{
		cObj->position.y = 2000;
		cObj->velocity.y = 0;
	}





	cObj->grid.x = (int) cObj->position.x/10;
	cObj->grid.y = (int) cObj->position.y/10;

	//Calculate how the shape warps. The shape will stretch as you move faster to avoid clipping
	cObj->warpedShape.x = ((abs(cObj->velocity.x)/10)+2)*(cObj->grid.shapex/10);
	cObj->warpedShape.y = ((abs(cObj->velocity.y)/10)+2)*(cObj->grid.shapey/10);

	//TODO: Increase object sGrid size for rotation as well. Might need to be pre-computed

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
				GE_CollisionFullCheck(cObj,allPhysicsObjects[sGrid[posx][posy]]);
			}
		}
	}
	/*for (int i=1;i < (nextPhysicsObject); i++) //uncomment to isolate collision checker from sGrid checker
	{
		if (allPhysicsObjects[i] != cObj)
		{
			GE_CollisionFullCheck(cObj,allPhysicsObjects[i]);
		}
	}*/
	
	cObj->lastGoodPosition = cObj->position;
}
void GE_CollisionFullCheck(PhysicsObject* cObj, PhysicsObject* victimObj)
{
	for (int a=0; a < cObj->numCollisionRectangles;a++)
	{
		//iterate through each of our rectangles...

		Vector2 myPoints[4] = {};
		GE_RectangleToPoints(cObj->collisionRectangles[a],myPoints,cObj->position);

		for (int b=0; b < victimObj->numCollisionRectangles;b++)
		{

			Vector2 theirPoints[4] = {};
			GE_RectangleToPoints(victimObj->collisionRectangles[b],theirPoints,victimObj->position);
			

			//iterate through each of my points, checking it against each victim point.
			for (int me = 0;me < 4; me++)
			{
				
				bool check1 = (myPoints[me].x >= theirPoints[0].x) && (myPoints[me].y >= theirPoints[0].y);
				bool check2 = (myPoints[me].x <= theirPoints[3].x) && (myPoints[me].y <= theirPoints[3].y);
				if (cObj->callCallbackBeforeCollisionFunction)
				{
					if (cObj->C_Collision(cObj,victimObj))
					{
						return;
					}
				}
				if (victimObj->callCallbackBeforeCollisionFunction)
				{
					if (victimObj->C_Collision(victimObj,cObj))
					{
						return;
					}

				}

				if (check1 && check2)
				{
					numCollisionsTemp++;
					std::cout << "FULL COLLISION DETECTED #" << numCollisionsTemp << std::endl;

					Vector2r newVelocity;

					/*newVelocity.x = (cObj->velocity.x*.5)+(victimObj->velocity.x*.5);
					newVelocity.y = (cObj->velocity.y*.5)+(victimObj->velocity.y*.5);
					newVelocity.r = (cObj->velocity.r*.5)+(victimObj->velocity.r*.5);*/

					//velocity should be based on the _difference_ of the two velocities
					
					//newVelocity = (cObj->velocity-victimObj->velocity)*0.5;





					/*cObj->velocity.x = (cObj->velocity.x*.5)+(victimObj->velocity.x*(-0.5));
					cObj->velocity.y = (cObj->velocity.y*.5)+(victimObj->velocity.y*(-0.5));
					cObj->velocity.r = (cObj->velocity.r*.5)+(victimObj->velocity.r*(-0.5));


					victimObj->velocity.x = (victimObj->velocity.x*(-0.5))+(oldVelocity.x*(0.5));
					victimObj->velocity.y = (victimObj->velocity.y*(-0.5))+(oldVelocity.y*(0.5));
					victimObj->velocity.r = (victimObj->velocity.r*(-0.5))+(oldVelocity.r*(0.5));*/

					/*cObj->velocity.x = newVelocity.x*-1;
					cObj->velocity.y = newVelocity.y*-1;
					cObj->velocity.r = newVelocity.r*-1;
					cObj->velocity = newVelocity;
					//cObj->position += newVelocity;

					victimObj->velocity.x = newVelocity.x*-1;
					victimObj->velocity.y = newVelocity.y*-1;
					victimObj->velocity.r = newVelocity.r*-1;
					*/
				/*	newVelocity.x = (victimObj->velocity.x-cObj->velocity.x)*0.5;
					newVelocity.y = (victimObj->velocity.y-cObj->velocity.y)*0.5;
					newVelocity.r = (victimObj->velocity.r-cObj->velocity.r)*0.5;


					Vector2r otherNewVelocity;

					otherNewVelocity.x = (cObj->velocity.x-victimObj->velocity.x)*0.5;
					otherNewVelocity.y = (cObj->velocity.y-victimObj->velocity.y)*0.5;
					otherNewVelocity.r = (cObj->velocity.r-victimObj->velocity.r)*0.5;

					std::cout << "x " << otherNewVelocity.x << " y " << otherNewVelocity.y << " r " << otherNewVelocity.r << std::endl;

					victimObj->velocity.x -= otherNewVelocity.x;
					victimObj->velocity.y -= otherNewVelocity.y;
					victimObj->velocity.r -= otherNewVelocity.r;
					
					cObj->velocity.x -= newVelocity.x;
					cObj->velocity.y -= newVelocity.y;
					cObj->velocity.r -= newVelocity.r;
*/
					/*cObj->velocity = cObj->velocity+newVelocity;

					victimObj->velocity = victimObj->velocity-newVelocity;
*/
					//SDL_Delay(250);

					/*cObj->position = cObj->position-cObj->velocity;
					victimObj->position = victimObj->position-victimObj->velocity;
					//I was completely expecting this to cause everything to flip its shit BUT IT WORKS REALLY WELL! Certified Good Enough!
					//just kidding it flips its shit
					*/

					//SDL_Delay(256);
					
					
					cObj->position = cObj->lastGoodPosition;
					victimObj->position = victimObj->lastGoodPosition;


					Vector2r cObjsVelocity = cObj->velocity;
					cObj->velocity = victimObj->velocity;
					victimObj->velocity = cObjsVelocity;


					//TODO: is it possible that the object order (the victimObj's tick might not have occured yet) could be causing our troubles?

					if (cObj->callCallbackAfterCollisionFunction)
					{
						if (cObj->C_Collision(cObj,victimObj))
						{
							//return TODO when collisionCheck is branched to a seperate function
						}
					}
					if (victimObj->callCallbackAfterCollisionFunction)
					{
						if (victimObj->C_Collision(victimObj,cObj))
						{
							//return
						}

					}
					return;
				}
				else
				{
					//std::cout << "No collision detected" << std::endl;
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

#ifdef physics_debug

	SDL_Renderer* debugRenderer;
	Camera* debugCamera;

	void GE_DEBUG_PassRendererToPhysicsEngine(SDL_Renderer* yourRenderer, Camera* yourCamera)
	{
		debugRenderer = yourRenderer;
		debugCamera = yourCamera;
	}
#endif


void GE_RectangleToPoints(Rectangle rect, Vector2* points, Vector2r hostPosition)
{

	points[0] = {rect.x , rect.y}; //top left
	points[1] = {rect.x+rect.w , rect.y}; //top right
	points[2] = {rect.x , rect.y+rect.h}; //bottom left
	points[3] = {rect.x+rect.w , rect.y+rect.h}; //bottom right

	double halfrectw = rect.w/2;
	double halfrecth = rect.h/2;
	for (int i =0; i < 4; i++)
	{
		points[i].x -= halfrectw;
		points[i].y -= halfrecth;
		GE_Vector2RotationCCW(&points[i],hostPosition.r);
		points[i].x += halfrectw+hostPosition.x;
		points[i].y += halfrecth+hostPosition.y;


		#ifdef physics_debug

			//note that the physics debugRender does NOT include rotation of the screen, and probably never will. it is good enough for its purpose.
			SDL_SetRenderDrawColor( debugRenderer, 0xFF, 0xFF, 0x00, 0xFF ); 

			SDL_Rect debugRect;

			debugRect.x = points[i].x;
			debugRect.y = points[i].y;
			debugRect.w = 3;
			debugRect.h = 3;

			//counter for the 3-sized rectangle
			debugRect.x -=1;
			debugRect.y -=1;

			debugRect.x -= (debugCamera->pos.x-debugCamera->screenWidth/2);
			debugRect.y -= (debugCamera->pos.y-debugCamera->screenHeight/2);

			SDL_RenderFillRect( debugRenderer, &debugRect ); 
		#endif

	}
}
