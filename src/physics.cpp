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

std::function< void ()> C_PhysicsTickPreCallbacks[MAX_PHYSICS_ENGINE_PRE_CALLBACKS];
int numPhysicsTickPreCallbacks = -1;


GE_PhysicsObject::GE_PhysicsObject(Vector2r position, Vector2r velocity, GE_Rectangle grid)
{
	numFakePhysicsIDs++;
	numPhysicsObjects++;

	this->position = position;
	this->velocity = velocity;
	this->grid = grid; //TODO automate 

	warpedShape = {0,0};

	ID = numFakePhysicsIDs;

	numCollisionRectangles = 0;

	lastGoodPosition = position;


	//TODO temp
	callCallbackAfterCollisionFunction = false;
	callCallbackBeforeCollisionFunction = false;

	//end temp
	
	fakeToRealPhysicsID[numFakePhysicsIDs] = numPhysicsObjects;
	physicsObjects[numPhysicsObjects] = this;

	callCallbackUpdate = false;
	type = 0;
}
bool GE_PhysicsObject::C_Update()
{
	printf("!!Shouldn't be called!!\n");
	return false;
}
bool GE_PhysicsObject::C_Collision(int victimID, int collisionRectangleID)
{
	return false;
}




int GE_PhysicsInit()
{
	pthread_create(&PhysicsEngineThread,NULL,GE_physicsThreadMain,NULL );
	return 0;
}
GE_PhysicsObject* GE_CreatePhysicsObject(Vector2r newPosition, Vector2r newVelocity, Vector2 shape)
{
	GE_NoGreaterThan_NULL(numPhysicsObjects,MAX_PHYSICS_OBJECTS);
	//TODO: Auto-generate shape.

	GE_PhysicsObject* newPhysicsObject = new GE_PhysicsObject(newPosition, newVelocity, {0,0,shape.x, shape.y});

	fakeToRealPhysicsID[numFakePhysicsIDs] = numPhysicsObjects;
	physicsObjects[numPhysicsObjects] = newPhysicsObject;


	deadPhysicsObjects[numPhysicsObjects] = false;
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
 

int GE_AddPhysicsDoneCallback(std::function<void ()> callback)
{
	GE_NoGreaterThan(numPhysicsTickDoneCallbacks,MAX_PHYSICS_ENGINE_DONE_CALLBACKS);
	C_PhysicsTickDoneCallbacks[numPhysicsTickDoneCallbacks+1] = callback;
	numPhysicsTickDoneCallbacks++; //TODO: Is this safe to do while the physics thread is running? It shouldn't cause issues related to calling something that's nonexistant, because we add it first...
	return 0;

}
int GE_AddPhysicsPreCallback(std::function<void ()> callback)
{
	GE_NoGreaterThan(numPhysicsTickDoneCallbacks,MAX_PHYSICS_ENGINE_PRE_CALLBACKS);
	C_PhysicsTickPreCallbacks[numPhysicsTickPreCallbacks+1] = callback;
	numPhysicsTickPreCallbacks++; //TODO: Is this safe to do while the physics thread is running? It shouldn't cause issues related to calling something that's nonexistant, because we add it first...
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
	physicsObject->velocity.x = physicsObject->velocity.x+moreVelocity.x;
	physicsObject->velocity.y = physicsObject->velocity.y+moreVelocity.y;
	physicsObject->velocity.r = physicsObject->velocity.r-moreVelocity.r;
}
void GE_AddRelativeVelocity(GE_PhysicsObject* physicsObject, Vector2r moreVelocity)
{
	printf("addrelvel\n");
	physicsObject->velocity.r = physicsObject->velocity.r-moreVelocity.r;
	physicsObject->velocity.addRelativeVelocity({moreVelocity.x,moreVelocity.y,physicsObject->position.r}); //TODO: De-OOify
}


 
void* GE_physicsThreadMain(void *)
{
	struct timeval pt,nt;
	while(true)
	{
		gettimeofday(&pt, NULL);
		//printf("next time to run %ld\n",nextTimeToRun);
		
		pthread_mutex_lock(&PhysicsEngineMutex);	
		//printf("Lock physen\n");
		
		for (int i=0;i<numPhysicsTickPreCallbacks+1;i++)
		{
			C_PhysicsTickPreCallbacks[i]();
		}
		GE_TickPhysics();
		for (int i=0;i<numPhysicsTickDoneCallbacks+1;i++)
		{
			C_PhysicsTickDoneCallbacks[i]();
		}
		pthread_mutex_unlock(&PhysicsEngineMutex);
		//wait the rest of the 16.6ms
		gettimeofday(&nt, NULL);

		//printf("usleep%ld\n",16666-(nt.tv_usec-pt.tv_usec));
		usleep(fmax(16666-(nt.tv_usec-pt.tv_usec),0));
		

	}
}


int numCollisionsTemp = 0;
void GE_TickPhysics()
{
	for (int i=0;i < (numPhysicsObjects+1); i++)
	{
		//printf("i %d\n",i);
		if(!deadPhysicsObjects[i])
		{
			if (!GE_TickPhysics_ForObject(physicsObjects[i],i))
			{
				if(physicsObjects[i]->callCallbackUpdate)
				{
					physicsObjects[i]->C_Update();
				}
			}
		}
		//printf("x %d y %d\n",physicsObjects[i]->position.x,physicsObjects[i]->position.y);
	}
}
//TODO: Factor in the fakeID, move the sGrid-removal to a seperate function called upon physicsObject death
bool GE_TickPhysics_ForObject(GE_PhysicsObject* cObj, int ID)
{
	//move ourselves forward
	//first set velocity
	
	cObj->position = cObj->position+cObj->velocity;


	cObj->grid.x = (int) (cObj->position.x/10);
	cObj->grid.y = (int) (cObj->position.y/10);

	//Calculate how the shape warps. The shape will stretch as you move faster to avoid clipping
	cObj->warpedShape.x = ((abs(cObj->velocity.x)/10)+2)*(cObj->grid.w/10);
	cObj->warpedShape.y = ((abs(cObj->velocity.y)/10)+2)*(cObj->grid.h/10);



	for (int i=0;i < (numPhysicsObjects+1); i++)
	{
		//printf("i %d\n",i);
		if(!deadPhysicsObjects[i] && i != ID)
		{
			if (GE_CollisionFullCheck(cObj,physicsObjects[i]))
			{
				return true;	
			}
		}
	}
	
	cObj->lastGoodPosition = cObj->position;
	return false;
}
bool GE_CollisionFullCheck(GE_PhysicsObject* cObj, GE_PhysicsObject* victimObj)
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
					bool killMe = false;
					bool doReturn = false;
					if (cObj->callCallbackBeforeCollisionFunction)
					{
						killMe = cObj->C_Collision(victimObj->ID,a);
						doReturn = true;
					}
					if (victimObj->callCallbackBeforeCollisionFunction)
					{
						if(victimObj->C_Collision(cObj->ID,b))
						{
							GE_FreePhysicsObject(victimObj);
							doReturn = true;
						}
					}
					if (killMe)
					{
						GE_FreePhysicsObject(cObj); 
					}
					
					if (doReturn)
					{
						return true;
					} 
				
					numCollisionsTemp++;
					std::cout << "FULL COLLISION DETECTED #" << numCollisionsTemp << std::endl;

					Vector2r newVelocity;
					cObj->position = cObj->lastGoodPosition;
					victimObj->position = victimObj->lastGoodPosition;


					Vector2r cObjsVelocity = cObj->velocity;
					cObj->velocity = victimObj->velocity;
					victimObj->velocity = cObjsVelocity;


					//TODO: is it possible that the object order (the victimObj's tick might not have occured yet) could be causing our troubles?

					if (cObj->callCallbackAfterCollisionFunction)
					{
						killMe = cObj->C_Collision(victimObj->ID,a); //reuse of variable; okay because of the return
						
					}
					if (victimObj->callCallbackAfterCollisionFunction)
					{
						if(victimObj->C_Collision(cObj->ID,b))
						{
							GE_FreePhysicsObject(victimObj);
							printf("~~~~~Kill      them\n");
						}
					}
					if (killMe)
					{
						printf("~~~~~KillMe\n");
						GE_FreePhysicsObject(cObj);
						return true;
					}
					return false;
				}
				else
				{
					//std::cout << "No collision detected" << std::endl;
				}
				
			}
		}
	}
	return false;
}


void GE_FreePhysicsObject(GE_PhysicsObject* physicsObject) //MUST be allocated with new
{
	deadPhysicsObjects[physicsObject->ID] = true;
	fakeToRealPhysicsID[physicsObject->ID] = -1;
	delete physicsObject;
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1test\n");
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

			pthread_mutex_lock(&RenderEngineMutex);
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
			pthread_mutex_unlock(&RenderEngineMutex);
		#endif

	}
}
