/*
Copyright 2017 Jackson Reed McNeill

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "physics.h"




pthread_t PhysicsEngineThread;

pthread_mutex_t PhysicsEngineMutex = PTHREAD_MUTEX_INITIALIZER;


GE_PhysicsObject* physicsObjects[MAX_PHYSICS_OBJECTS];
int numPhysicsObjects = -1; 
bool deadPhysicsObjects[MAX_PHYSICS_OBJECTS]; //TODO shift all elements down instead of this patch
int fakeToRealPhysicsID[MAX_PHYSICS_OBJECTS]; 
int numFakePhysicsIDs = -1;

std::function< void ()> C_PhysicsTickDoneCallbacks[MAX_PHYSICS_ENGINE_DONE_CALLBACKS];
int numPhysicsTickDoneCallbacks = -1;

int sGrid[2000][2000]; //TODO: Dynamically sized arrays for both of these
//int collisionSpots[2000][1]; 
//int nextCollisionSpot = 0;




int GE_PhysicsInit()
{
	pthread_create(&PhysicsEngineThread,NULL,GE_physicsThreadMain,NULL );
	return 0;
}
GE_PhysicsObject* GE_CreatePhysicsObject(Vector2r newPosition, Vector2r newVelocity, Vector2 shape)
{
	GE_NoGreaterThan_NULL(numPhysicsObjects,MAX_PHYSICS_OBJECTS);
	//TODO: Auto-generate shape.

	numFakePhysicsIDs++;
	numPhysicsObjects++;
	GE_PhysicsObject* newPhysicsObject = new GE_PhysicsObject{{0,0,0},{0,0,0},newPosition,true,newVelocity,true,{0,0,shape.x, shape.y},{0,0},numFakePhysicsIDs,{},0,{0,0,0},false};

	fakeToRealPhysicsID[numFakePhysicsIDs] = numPhysicsObjects;
	physicsObjects[numPhysicsObjects] = newPhysicsObject;
	return newPhysicsObject;
}

int GE_GetPhysicsObjectFromID(int fakeID, GE_PhysicsObject** physicsObjectPointer)
{
	if (fakeID > numFakePhysicsIDs)
	{
		return 1;
	}
	int physicsObjectID = fakeToRealPhysicsID[fakeID];
	if (physicsObjectID == -1)
	{
		return 1;
	}
	(*physicsObjectPointer) = physicsObjects[physicsObjectID];


	return 0;
}
 

void GE_AddPhysicsObjectCollisionCallback(GE_PhysicsObject* subject, std::function< bool (GE_PhysicsObject* cObj, GE_PhysicsObject* victimObj)> C_Collision, bool callCallbackBeforeCollisionFunction)
{
	subject->C_Collision = C_Collision;
	subject->callCallbackBeforeCollisionFunction = callCallbackBeforeCollisionFunction;
	subject->callCallbackAfterCollisionFunction = !callCallbackBeforeCollisionFunction;
}
int GE_AddPhysicsDoneCallback(std::function<void ()> callback)
{
	GE_NoGreaterThan(numPhysicsTickDoneCallbacks,MAX_PHYSICS_ENGINE_DONE_CALLBACKS);
	C_PhysicsTickDoneCallbacks[numPhysicsTickDoneCallbacks+1] = callback;
	numPhysicsTickDoneCallbacks++; //TODO: Is this safe to do while the physics thread is running? It shouldn't cause issues related to calling something that's nonexistant, because we add it first...
	return 0;

}


/*void GE_SetPosition(Vector2r newPosition)
{
	this->newPosition = newPosition;
	setNewPosition = true;
}
void GE_PhysicsObject::setVelocity(Vector2r newVelocity)
{
	this->newVelocity = newVelocity;
	setNewVelocity = true;
}*/
void GE_AddVelocity(GE_PhysicsObject* physicsObject, Vector2r moreVelocity)
{
	physicsObject->newVelocity.x = physicsObject->velocity.x+moreVelocity.x;
	physicsObject->newVelocity.y = physicsObject->velocity.y+moreVelocity.y;
	physicsObject->newVelocity.r = physicsObject->velocity.r-moreVelocity.r;
	physicsObject->setNewVelocity = true;
}
void GE_AddRelativeVelocity(GE_PhysicsObject* physicsObject, Vector2r moreVelocity)
{
	physicsObject->newVelocity = physicsObject->velocity;
	physicsObject->newVelocity.r = physicsObject->velocity.r-moreVelocity.r;
	physicsObject->newVelocity.addRelativeVelocity({moreVelocity.x,moreVelocity.y,physicsObject->position.r}); //TODO: De-OOify
	physicsObject->setNewVelocity = true;
}


void* GE_physicsThreadMain(void *)
{
	while(true)
	{
		pthread_mutex_lock(&PhysicsEngineMutex);	
		GE_TickPhysics();
		for (int i=0;i<numPhysicsTickDoneCallbacks+1;i++)
		{
			C_PhysicsTickDoneCallbacks[i]();
		}
		pthread_mutex_unlock(&PhysicsEngineMutex);

		SDL_Delay(16); //delay outside of mutex lock, else will hog the mutex and never let other sections run
	}
}


int numCollisionsTemp = 0;
void GE_TickPhysics()
{
	for (int i=0;i < (numPhysicsObjects+1); i++)
	{
		//printf("i %d\n",i);
		GE_TickPhysics_ForObject(physicsObjects[i],i);
		//printf("x %d y %d\n",physicsObjects[i]->position.x,physicsObjects[i]->position.y);
	}
}
//TODO: Factor in the fakeID, move the sGrid-removal to a seperate function called upon physicsObject death
void GE_TickPhysics_ForObject(GE_PhysicsObject* cObj, int ID)
{
	//remove our old grid 
	for (int x = 0; x < cObj->warpedShape.x; x++) 
	{
		for (int y = 0; y < cObj->warpedShape.y; y++) 
		{
			int posx = x+cObj->grid.x;
			int posy = y+cObj->grid.y;
			if (sGrid[posx][posy] == ID) //do not remove other's sGrid entries. if we did, we wouldn't ever detect collision.
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
	cObj->warpedShape.x = ((abs(cObj->velocity.x)/10)+2)*(cObj->grid.w/10);
	cObj->warpedShape.y = ((abs(cObj->velocity.y)/10)+2)*(cObj->grid.h/10);

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
				sGrid[posx][posy] = ID;
			}
			if (sGrid[posx][posy] != 0 && sGrid[posx][posy] != ID)
			{
				GE_CollisionFullCheck(cObj,physicsObjects[sGrid[posx][posy]]); //TODO: convert to realID
			}
		}
	}
	/*for (int i=1;i < (nextPhysicsObject); i++) //uncomment to isolate collision checker from sGrid checker
	{
		if (physicsObjects[i] != cObj)
		{
			GE_CollisionFullCheck(cObj,physicsObjects[i]);
		}
	}*/
	
	cObj->lastGoodPosition = cObj->position;
}
void GE_CollisionFullCheck(GE_PhysicsObject* cObj, GE_PhysicsObject* victimObj)
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

				if (check1 && check2)
				{
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
					numCollisionsTemp++;
					//std::cout << "FULL COLLISION DETECTED #" << numCollisionsTemp << std::endl;

					Vector2r newVelocity;
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
							return;
						}
					}
					if (victimObj->callCallbackAfterCollisionFunction)
					{
						if (victimObj->C_Collision(victimObj,cObj))
						{
							return;
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


void GE_FreePhysicsObject(GE_PhysicsObject* physicsObject) //MUST be allocated with new
{
	deadPhysicsObjects[physicsObject->ID] = true;
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


void GE_RectangleToPoints(GE_Rectangle rect, Vector2* points, Vector2r hostPosition) 
{
		
	//Points make collision checking easy because you cannot rotate a point in space (or rather, rotating it would have no effect)

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
