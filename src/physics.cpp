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

#include <unordered_set>


#ifdef PHYSICS_DEBUG_SLOWRENDERS

	bool DEBUG_allowPhysicsTick = true;

	bool DEBUG_isCObj = false;

#endif


//Config defaults 

double PhysicsDelaySeconds = 0.16667;


bool PhysicsEngineThreadShutdown = false;

pthread_t PhysicsEngineThread;

pthread_mutex_t PhysicsEngineMutex = PTHREAD_MUTEX_INITIALIZER;


std::list<GE_PhysicsObject*> AllPhysicsObjects;
int numPhysicsObjects = -1; 

std::map<physics_area_coord_t,physics_area_t*> world;  //TODO: Re-implemt the sGrid idea but with each area having its own sGrid?


#define checkNotDead(object) deadPhysicsObjects.find(object) == deadPhysicsObjects.end()


std::unordered_set<GE_PhysicsObject*> deadPhysicsObjects;

int ticknum = 0;

std::function< void ()> C_PhysicsTickDoneCallbacks[MAX_PHYSICS_ENGINE_DONE_CALLBACKS];
int numPhysicsTickDoneCallbacks = -1;

std::function< void ()> C_PhysicsTickPreCallbacks[MAX_PHYSICS_ENGINE_PRE_CALLBACKS];
int numPhysicsTickPreCallbacks = -1;


GE_PhysicsObject::GE_PhysicsObject(Vector2r position, Vector2r velocity, GE_Rectangle grid, double mass)
{
	numPhysicsObjects++;

	this->position = position;
	this->velocity = velocity;
	this->grid = grid; //TODO automate 
	this->mass = mass;

	warpedShape = {0,0};


	numCollisionRectangles = 0;
	numGlueTargets = -1;

	lastGoodPosition = position;


	//TODO temp
	callCallbackAfterCollisionFunction = false;
	callCallbackBeforeCollisionFunction = false;

	//end temp
	
	AllPhysicsObjects.insert(AllPhysicsObjects.end(),this);

	callCallbackUpdate = false;
	type = 0;
}
bool GE_PhysicsObject::C_Update()
{
	printf("!!Shouldn't be called!!\n");
	return false;
}
bool GE_PhysicsObject::C_Collision(GE_PhysicsObject* victim, int collisionRectangleID)
{
	return false;
}
GE_PhysicsObject::~GE_PhysicsObject()
{
	printf("NORMAL REMOVAL FUNCTION\n");
}




int GE_PhysicsInit()
{
	pthread_create(&PhysicsEngineThread,NULL,GE_physicsThreadMain,NULL );
	return 0;
}
void GE_LinkVectorToPhysicsObjectPosition(GE_PhysicsObject* subject, Vector2r* link)
{
	GE_LinkGlueToPhysicsObject(subject, GE_addGlueSubject(link,&subject->position,GE_PULL_ON_PHYSICS_TICK,sizeof(Vector2r)) );
}
void GE_LinkVectorToPhysicsObjectVelocity(GE_PhysicsObject* subject, Vector2r* link)
{
	GE_LinkGlueToPhysicsObject(subject, GE_addGlueSubject(link,&subject->velocity,GE_PULL_ON_PHYSICS_TICK,sizeof(Vector2r)) );
}
void GE_LinkGlueToPhysicsObject(GE_PhysicsObject* subject, GE_GlueTarget* glue)
{
	subject->numGlueTargets++;
	subject->glueTargets[subject->numGlueTargets] = glue;
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
	physicsObject->velocity.r = physicsObject->velocity.r-moreVelocity.r;
	physicsObject->velocity.addRelativeVelocity({moreVelocity.x,moreVelocity.y,physicsObject->position.r}); //TODO: De-OOify
}

double GE_GetUNIXTime()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	
	return static_cast<double>(tv.tv_sec)+(static_cast<double>(tv.tv_usec)/(static_cast<double>(10e6))); //Add the microseconds divded by 10e6 to end up with seconds.microseconds
	//Static casting intensifies
}


 
void* GE_physicsThreadMain(void *)
{
	double startTime, finishTime;
	while(GE_IsOn)
	{

#ifdef PHYSICS_DEBUG_SLOWRENDERS
		if (DEBUG_allowPhysicsTick) //For freezing the physics engine
		{
#endif
		
		startTime = GE_GetUNIXTime();
		
		//printf("Try lock physics engine\n");
		pthread_mutex_lock(&PhysicsEngineMutex);	
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
		/*printf("time them %f time now %f\n",(float)pt.tv_usec,(float) nt.tv_usec);
		printf("tick took %f\n",(float)(nt.tv_usec-pt.tv_usec));
		printf("Unlock physics engine sleep for %f\n",fmax(PhysicsDelaySeconds-(nt.tv_usec-pt.tv_usec),0));*/
		finishTime = GE_GetUNIXTime();
		double sleepTime = fmax((PhysicsDelaySeconds-(finishTime-startTime))*10e4,0);


		/*printf("delta time %f\n",(finishTime-startTime));
		printf("sleep time %f\n",sleepTime);
		printf("finish time %f\n",finishTime);*/
		usleep(sleepTime); //delay the physics engine thread to get a perfect 16.67ms/tick

		//10e4 is derrived from 10e6 (to take seconds and microseconds and produce seconds with decimals, eg. 1.656785) 
	

#ifdef PHYSICS_DEBUG_SLOWRENDERS
		}
		SDL_Delay(1); //prevent from taking up a whole core, sdl is available to physics engine in debug mode
#endif

		

	}
	printf("Physics Engine thread is shutting down.\n");
	PhysicsEngineThreadShutdown = true;
	return (void*)NULL;
}

//really deletes the object instead of queing for deletion
void GE_FreePhysicsObject_InternalFullDelete(GE_PhysicsObject* physicsObject) //MUST be allocated with new
{
	printf("I AM DELETING ID #%d\n",physicsObject->ID);
	printf("num glues %d",physicsObject->numGlueTargets);

	AllPhysicsObjects.remove(physicsObject);

	for (int i=0;i<=physicsObject->numGlueTargets;i++)
	{
		printf("le %d\n",i);
		GE_FreeGlueObject(physicsObject->glueTargets[i]);
		physicsObject->glueTargets[i] = NULL;
	}
	printf("Deleting physics object itself...\n");
	delete physicsObject;
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1test\n");
}

int temp2 = 0;

int numCollisionsTemp = 0;
void GE_TickPhysics()
{
temp2 = 0;
	ticknum++;
	printf("Physics tick #%d\n",ticknum);
	
	for (GE_PhysicsObject* object : AllPhysicsObjects)
	{
		if (checkNotDead(object))
		{
			if (!GE_TickPhysics_ForObject(object))
			{
				if(object->callCallbackUpdate)
				{
					object->C_Update();
				}
			}
		}
		//printf("x %d y %d\n",AllPhysicsObjects[i]->position.x,AllPhysicsObjects[i]->position.y);
	}

	std::unordered_set<GE_PhysicsObject*>::iterator it;
	while(true) //erase stuff like a queue, cause we're constantly removing things and shifting the iterators arround, this works best
	{
		it = deadPhysicsObjects.begin();
		if (it == deadPhysicsObjects.end())
		{
			break;
		}

		GE_FreePhysicsObject_InternalFullDelete(*it);
		deadPhysicsObjects.erase(it);
	}
	printf("this tick had %d collision full checks\n",temp2);
}

struct InternalResult
{
	bool deleteMe;
	bool didCollide;
};
unsigned int temp;
InternalResult GE_CollisionFullCheck(GE_PhysicsObject* cObj, GE_PhysicsObject* victimObj)
{
	temp++;
	temp2++;
	//printf("Collision full check #%d (this tick: %d)\n",temp,temp2);

	Vector2 theirPoints[4] = {};
	Vector2 myPoints[4] = {};
	for (int a=0; a < cObj->numCollisionRectangles;a++)
	{
		//iterate through each of our rectangles...

#ifdef PHYSICS_DEBUG_SLOWRENDERS
		DEBUG_isCObj = true;
#endif
		GE_RectangleToPoints(cObj->collisionRectangles[a],cObj->grid,myPoints,cObj->position);

		for (int b=0; b < victimObj->numCollisionRectangles;b++)
		{

#ifdef PHYSICS_DEBUG_SLOWRENDERS
			DEBUG_isCObj = false;
#endif
			GE_RectangleToPoints(victimObj->collisionRectangles[b],victimObj->grid,theirPoints,victimObj->position);
			

			//iterate through each of my points, checking it against each victim point.
			for (int me = 0;me < 4; me++)
			{
				
				
				/*bool check0 = (myPoints[me].x >= theirPoints[0].x) && (myPoints[me].y >= theirPoints[0].y);
				bool check1 = true;//(myPoints[me].x >= theirPoints[1].x) && (myPoints[me].y <= theirPoints[1].y);
				bool check2 = (myPoints[me].x <= theirPoints[2].x) && (myPoints[me].y >= theirPoints[2].y);
				bool check3 = true;//(myPoints[me].x <= theirPoints[3].x) && (myPoints[me].y >= theirPoints[3].y);*/

				bool check = false;

				//Algorithm credits: Raymond Manzoni (https://math.stackexchange.com/users/21783/raymond-manzoni), How to check if a point is inside a rectangle?, URL (version: 2012-09-03): https://math.stackexchange.com/q/190373

				Vector2 AM = myPoints[me]-theirPoints[0]; //M-A
				Vector2 AB = theirPoints[1]-theirPoints[0];
				Vector2 AD = theirPoints[2]-theirPoints[0]; //bottom-left minus top left

				/*printf("AM %f,%f\n",AM.x,AM.y);
				printf("dot %f\n",GE_Dot(AM,AB));
				printf("check pt1 %d\n",( 0.0 < GE_Dot(AM,AB) < GE_Dot(AB,AB) ));*/

				double AMAB = GE_Dot(AM,AB);
				double AMAD = GE_Dot(AM,AD);

				check = ( ( 0.0 <= AMAB ) && ( AMAB <= GE_Dot(AB,AB) ) && ( 0.0 <= AMAD ) && ( AMAD <= GE_Dot(AD,AD) ) ) ;


				

				if (check)
				{
					bool killMe = false;
					bool doReturn = false;
					if (cObj->callCallbackBeforeCollisionFunction)
					{
						killMe = cObj->C_Collision(victimObj,a);
						doReturn = true;
						printf("cobj wants before\n");
					}
					if (victimObj->callCallbackBeforeCollisionFunction)
					{
						if(victimObj->C_Collision(cObj,b))
						{
							printf("victim die\n");
							GE_FreePhysicsObject(victimObj);
						}
						doReturn = true;
						printf("victim wants before\n");
					}
					if (killMe)
					{
						printf("cobj die\n");
						GE_FreePhysicsObject(cObj); 
						return {true,true};
					}
					
					if (doReturn)
					{
						printf("no die\n");
						return {false,false};
					} 
				
					numCollisionsTemp++;
					std::cout << "FULL COLLISION DETECTED #" << numCollisionsTemp << std::endl;

					cObj->position = cObj->lastGoodPosition;

					victimObj->position = victimObj->lastGoodPosition;



					//TODO: Add the inverse of the speed given to the other object in order to have conservation of energy (specifically: not duplicating the energy transfered).

					Vector2r cObjVelocity = cObj->velocity;
					
					Vector2r momentum = ((cObj->velocity*cObj->mass)+(victimObj->velocity*victimObj->mass))/2;
					
					printf("momentum %f,%f\n",momentum.x,momentum.y);

					GE_InelasticCollision(cObj,myPoints[me],momentum,true);
					
					//newVelocity = cObjVelocity*cObj->mass;//GE_InelasticCollisionVelocityExchange(cObj->velocity,victimObj->velocity,cObj->mass,victimObj->mass);
					momentum = ((cObjVelocity*cObj->mass)/victimObj->mass);
					
					GE_InelasticCollision(victimObj,myPoints[me],momentum,false);







					//TODO: is it possible that the object order (the victimObj's tick might not have occured yet) could be causing our troubles?

					if (cObj->callCallbackAfterCollisionFunction)
					{
						killMe = cObj->C_Collision(victimObj,a); //reuse of variable; okay because of the return
						
					}
					if (victimObj->callCallbackAfterCollisionFunction)
					{
						if(victimObj->C_Collision(cObj,b))
						{
							GE_FreePhysicsObject(victimObj);
							printf("~~~~~Kill      them\n");
						}
					}
					if (killMe)
					{
						printf("~~~~~KillMe\n");
						GE_FreePhysicsObject(cObj);
						return {true,true};
					}
					return {false,true};
				}
				else
				{
					//std::cout << "No collision detected" << std::endl;
				}
				
			}
		}
	}
	return {false,false};
}
InternalResult GE_TickPhysics_ForObject_Internal(GE_PhysicsObject* cObj, Vector2r* velocity)
{
	//move ourselves forward
	cObj->position = cObj->position+(*velocity);


	//remove us from the old areas first
	

	//TODO: I'm imaging a syste where this happens in GE_TickPhysics_ForObject and predicts where it will be if it doesn't have a collision, and puts the object in those areas. 
	
	physics_object_area_list_t::iterator it;
	while (true)
	{
		it = cObj->areas.begin();
		if (it == cObj->areas.end())
		{
			break;
		}
		(*it)->remove(cObj);
		cObj->areas.erase(it);
	}

	physics_area_single_coord_t xMin = (cObj->position.x/PHYSICS_AREA_SIZE);
	physics_area_single_coord_t yMin = (cObj->position.y/PHYSICS_AREA_SIZE);

	physics_area_single_coord_t xMax = ((cObj->position.x+cObj->grid.w)/PHYSICS_AREA_SIZE);
	physics_area_single_coord_t yMax = ((cObj->position.y+cObj->grid.h)/PHYSICS_AREA_SIZE);


	for(physics_area_single_coord_t x=xMin;x<=xMax;x++)
	{
		for (physics_area_single_coord_t y=yMin;y<=yMax;y++)
		{
#ifdef PHYSICS_DEBUG_SLOWRENDERS
			GE_DEBUG_TextAt_PhysicsPosition(std::to_string(x)+", y "+std::to_string(y),cObj->position);
#endif
			physics_area_coord_t coord = physics_area_coord_t(x,y);
			auto area_it = world.find(coord);
			physics_area_t* area = area_it->second;
			if (area_it == world.end())
			{
				world[coord]  = new physics_area_t();
				area = world[coord];
				printf("New map spot!\n");
			}
			

			area->push_front(cObj);

			cObj->areas.insert(cObj->areas.begin(),area);
		}
	}


	

	int temp3 = 0;
	for (physics_area_t* area : cObj->areas)
	{

		for (auto it = area->begin(); it != area->end();it++)
		{
			temp3++;
			GE_PhysicsObject* victimObj = *it;
			if(victimObj != cObj && checkNotDead(victimObj))
			{
				double maxSize = fmax(cObj->grid.w, cObj->grid.h); //fmax tested to be about 2x faster than std::max in this situation
				double theirMaxSize = fmax(victimObj->grid.w, victimObj->grid.h);
				if ( ( cObj->position.x+maxSize >= victimObj->position.x-theirMaxSize) && (cObj->position.x-maxSize <= victimObj->position.x+theirMaxSize) && (cObj->position.y+maxSize >= victimObj->position.y-theirMaxSize) && (cObj->position.y-maxSize <=victimObj->position.y+theirMaxSize)) //tested to help a lot as compared to just running a full check.
				{
					InternalResult result = GE_CollisionFullCheck(cObj,victimObj);
					if (result.deleteMe || result.didCollide)
					{
						return result;
					}
				}
			}
		}
	}
	//printf("TOTAL CHECKS: %d\n",temp3);
	
	cObj->lastGoodPosition = cObj->position;
	return {false,false};
}
bool GE_TickPhysics_ForObject(GE_PhysicsObject* cObj)
{
	//To avoid Clipping/"Bullet through paper" effect, we will slow down the physics simulation for a specific object by an unsigned nonzero integer and then slow down velocity accordingly. The user SHOULD NOT be able to tell any of this is happening. For performance, velocity is passed as a pointer to the internal function.
	
	unsigned int miniTickrate = fmax(ceil(  ((abs(cObj->velocity.x)+abs(cObj->velocity.y))/PHYSICS_MAX_SPEED_BEFORE_BROKEN_INTO_MINI_TICKS ) ),1.0); //minimum of 1. //TODO adjust this to good value
	
	
	Vector2r* velocity = new Vector2r{cObj->velocity.x/miniTickrate, cObj->velocity.y/miniTickrate, cObj->velocity.r/miniTickrate};

	//printf("MINITICKRATE: %d\n,",miniTickrate);


	for (int i = 0; i <= miniTickrate; i++)
	{//TODO: Because of this mini-tick system, objects that collide with a i/miniTickrate value of <1 will have non-up-to-date velocities being added. 
		InternalResult result = GE_TickPhysics_ForObject_Internal(cObj,velocity); 
		if (result.deleteMe)
		{
			printf("deleteme\n");
			return true;
		}
		else if (result.didCollide)
		{
			printf("BREAK but not\n");
			//break;
			velocity = new Vector2r{cObj->velocity.x/miniTickrate, cObj->velocity.y/miniTickrate, cObj->velocity.r/miniTickrate}; //TODO: Velocity could theoreticaly increase by 1,000 times then clip through something. recalculate mini-tickrate
		}
	}
	delete velocity;
	return false;

}



void GE_FreePhysicsObject(GE_PhysicsObject* physicsObject)
{
	deadPhysicsObjects.insert(physicsObject);
}



void GE_RectangleToPoints(GE_Rectangle rect, GE_Rectangle grid, Vector2* points, Vector2r hostPosition) 
{
		
	//Points make collision checking easy because you cannot rotate a point in space (or rather, rotating it would have no effect)

	points[0] = {rect.x , rect.y}; //top left
	points[1] = {rect.x+rect.w , rect.y}; //top right
	points[2] = {rect.x , rect.y+rect.h}; //bottom left
	points[3] = {rect.x+rect.w , rect.y+rect.h}; //bottom right

	double halfrectw = grid.w/2;//rect.w/2; //TODO I think this needs to be the size of the full object?
	double halfrecth = grid.h/2;//rect.h/2;
	for (int i =0; i < 4; i++)
	{
		points[i].x -= halfrectw;
		points[i].y -= halfrecth;
		GE_Vector2RotationCCW(&points[i],hostPosition.r);
		points[i].x += halfrectw+hostPosition.x;
		points[i].y += halfrecth+hostPosition.y;


		#ifdef PHYSICS_DEBUG_SLOWRENDERS

			pthread_mutex_lock(&RenderEngineMutex);
			//note that the physics debugRender does NOT include rotation of the screen, and probably never will. it is good enough for its purpose.
			SDL_SetRenderDrawColor( GE_DEBUG_Renderer, 0xFF, 0x00, 0x00, 0xFF ); 
			if (DEBUG_isCObj)
				SDL_SetRenderDrawColor( GE_DEBUG_Renderer, 0xFF, 0xFF, 0x00, 0xFF ); 

			SDL_SetRenderDrawColor( GE_DEBUG_Renderer, 0xFF, rect.w*3,rect.h*3, 0xFF ); 

			SDL_Rect debugRect;

			debugRect.x = points[i].x;
			debugRect.y = points[i].y;
			debugRect.w = 3;
			debugRect.h = 3;

			//counter for the 3-sized rectangle
			debugRect.x -=1;
			debugRect.y -=1;

			debugRect.x -= (GE_DEBUG_Camera->pos.x-GE_DEBUG_Camera->screenWidth/2);
			debugRect.y -= (GE_DEBUG_Camera->pos.y-GE_DEBUG_Camera->screenHeight/2);

			SDL_RenderFillRect( GE_DEBUG_Renderer, &debugRect ); 
			pthread_mutex_unlock(&RenderEngineMutex);
		#endif

	}
}

Vector2r GE_InelasticCollisionVelocityExchange(Vector2r velocity1, Vector2r velocity2, double mass1, double mass2) 
{
	return Vector2r{(mass1*velocity1.x+mass2*velocity2.x)/(mass1+mass2),(mass1*velocity1.y+mass2*velocity2.y)/(mass1+mass2),0};
}

Vector2 GE_GetRectangleCenterRealPosition(GE_Rectangle rectangle, Vector2r realPosition)
{
	return Vector2{rectangle.x+realPosition.x+(rectangle.w/2),rectangle.y+realPosition.y+(rectangle.h/2)};
}

void GE_InelasticCollision(GE_PhysicsObject* subject, Vector2 collisionPoint, Vector2r momentum, bool CCW) //TODO deg rad fixes
{
	//newVelocity = newVelocity - subject->velocity;

	//find the point of collision
	
	//Vector2 collisionPoint = myPoints[me];

	printf("collisionPoint %f,%f\n",collisionPoint.x,collisionPoint.y);


	//find the angle to apply the velocity at
	
	Vector2 centerPoint = GE_GetRectangleCenterRealPosition(subject->grid, subject->position);

	printf("centerPoint %f,%f\n",centerPoint.x,centerPoint.y);

	double adjacent = std::cos(collisionPoint.x-centerPoint.x)/(GE_Distance(collisionPoint,centerPoint));


	//adjacent *= RAD_TO_DEG;

	printf("adjacent %f\n (appx %f deg)\n",adjacent,adjacent*57.295779524);

	//subject->velocity = Vector2r{0,0,0};
	if (!CCW) adjacent += M_PI; //TODO temp
	subject->velocity.addRelativeVelocity(Vector2r{momentum.x/subject->mass,momentum.y/subject->mass,adjacent});
	
	printf("new velocity real %f, %f, %f\n",subject->velocity.x,subject->velocity.y,subject->velocity.r);
}

void GE_ShutdownPhysicsEngine()
{
	pthread_mutex_lock(&PhysicsEngineMutex);
	for (GE_PhysicsObject* object : AllPhysicsObjects)
	{
		GE_FreePhysicsObject(object);
	}
	pthread_mutex_unlock(&PhysicsEngineMutex);
}
