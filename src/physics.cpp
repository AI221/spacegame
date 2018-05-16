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

#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <map>
//#include <copy>

//Local includes

#include "GeneralEngineCPP.h"
#include "isOn.h"
#include "threadeddelete.h"


#include <unordered_set>


#ifdef PHYSICS_DEBUG_SLOWRENDERS

	bool DEBUG_allowPhysicsTick = true;

	bool DEBUG_isCObj = false;

#endif

bool GE_PhysicsEngine_CollisionsEnabled = true;
bool GE_PhysicsEngine_TickingObjectsEnabled = true;

//Config defaults 

double PhysicsDelaySeconds = 0.1666666666666666666;


std::atomic<bool> PhysicsEngineThreadShutdown(false);

pthread_t PhysicsEngineThread;

pthread_mutex_t PhysicsEngineMutex = PTHREAD_MUTEX_INITIALIZER;


std::list<GE_PhysicsObject*> allPhysicsObjects;
int numPhysicsObjects = 0; 


std::map<physics_area_coord_t,physics_area_t*> world;  //TODO: Re-implemt the sGrid idea but with each area having its own sGrid?

GE_ThreadedDelete<GE_PhysicsObject>* deleter;



#define checkNotDead(object) (deadPhysicsObjects.find(object) == deadPhysicsObjects.end())


std::unordered_set<GE_PhysicsObject*> deadPhysicsObjects;

int ticknum = 0;

std::function< void ()> C_PhysicsTickDoneCallbacks[MAX_PHYSICS_ENGINE_DONE_CALLBACKS];
int numPhysicsTickDoneCallbacks = -1;

std::function< void ()> C_PhysicsTickPreCallbacks[MAX_PHYSICS_ENGINE_PRE_CALLBACKS];
int numPhysicsTickPreCallbacks = -1;



#ifdef PHYSICS_DEBUG_SLOWRENDERS
void localdebug_worldsectors()
{

#define limit 5
	for (physics_area_single_coord_t x=-limit;x<limit;x++)
	{
		for (physics_area_single_coord_t y = -limit; y<limit;y++)
		{
			Vector2 vec = {static_cast<double>(x*PHYSICS_AREA_SIZE),static_cast<double>(y*PHYSICS_AREA_SIZE)};
			GE_DEBUG_TextAt_PhysicsPosition(GE_DEBUG_VectorToString(vec),vec);
			GE_DEBUG_DrawRect_PhysicsPosition(GE_Rectangle{vec.x,vec.y,PHYSICS_AREA_SIZE,PHYSICS_AREA_SIZE});
		}
	}

}
#endif 

GE_PhysicsObject::GE_PhysicsObject(Vector2r position, Vector2r velocity, double mass)
{

	this->position = position;
	this->velocity = velocity;
	this->grid = {0,0};
	this->mass = mass;

	warpedShape = {0,0};


	numCollisionRectangles = 0;
	numGlueTargets = -1;

	lastGoodPosition = position;


	//TODO temp
	callCallbackAfterCollisionFunction = false;
	callCallbackBeforeCollisionFunction = false;

	//end temp
	
	allPhysicsObjects.insert(allPhysicsObjects.end(),this);

	callCallbackUpdate = false;
	type = 0;

	centerOfMass = {0,0}; //set to geometric center

	this->ID = numPhysicsObjects;

	numPhysicsObjects++;
}
bool GE_PhysicsObject::C_Update()
{
	printf("!!Shouldn't be called!!\n");
	return false;
}
bool GE_PhysicsObject::C_Collision(GE_PhysicsObject* UNUSED(victim), int UNUSED(collisionRectangleID))
{
	return false;
}
void GE_PhysicsObject::addCollisionRectangle(GE_Rectangler newRectangle)
{
	//add rectangle
	collisionRectangles[numCollisionRectangles] = newRectangle;
	numCollisionRectangles++;

	//update cached values
	Vector2 myPoints[4] = {};
	grid = {0,0};
	///Vector2 minPoint = {0,0};
	for (int i=0;i<numCollisionRectangles;i++)
	{
		GE_RectangleToPoints(collisionRectangles[i],Vector2{0,0},myPoints,Vector2r{0,0,0});
		for (int o=0;o<4;o++)
		{
			grid.x = fmax(grid.x,myPoints[o].x);
			grid.y = fmax(grid.y,myPoints[o].y);

			//minPoint.x = fmin(minPoint.x,myPoints[o].x);
			//minPoint.y = fmin(minPoint.y,myPoints[o].y);
		}
	}
	diameter = fmax(grid.x,grid.y);
	radius = diameter/2;
}
GE_PhysicsObject::~GE_PhysicsObject()
{
}




int GE_PhysicsInit()
{
	pthread_create(&PhysicsEngineThread,NULL,GE_physicsThreadMain,NULL );
	deleter = new GE_ThreadedDelete<GE_PhysicsObject>;
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


//really deletes the object instead of queing for deletion
void internal_full_delete(GE_PhysicsObject* physicsObject) //MUST be allocated with new
{

	allPhysicsObjects.remove(physicsObject);
	
	//remove the object from the areas it's in
	physics_object_area_list_t::iterator it;
	while (true)
	{
		it = physicsObject->areas.begin();
		if (it == physicsObject->areas.end())
		{
			break;
		}
		(*it)->remove(physicsObject);
		physicsObject->areas.erase(it);
	}

	
	//delete physicsObject;
	deleter->queue_delete(physicsObject);
}
void internal_physics_object_gc_cycle()
{
	std::unordered_set<GE_PhysicsObject*>::iterator it;
	while(!deadPhysicsObjects.empty()) //erase stuff like a queue, cause we're constantly removing things and shifting the iterators arround, this works best
	{
		it = deadPhysicsObjects.begin();
		if (it == deadPhysicsObjects.end())
		{
			break;
		}

		internal_full_delete(*it);
		deadPhysicsObjects.erase(it);
	}

	if(wraparround_clamp(ticknum,60)==0)
	{
		deleter->force_cylce();
	}
}


 
#include <SDL2/SDL.h>
void* GE_physicsThreadMain(void *)
{
	double startTime, finishTime;
	while(GE_IsOn.load())
	{

#ifdef PHYSICS_DEBUG_SLOWRENDERS
		if (DEBUG_allowPhysicsTick) //For freezing the physics engine
		{
#endif
		
		startTime = GE_GetUNIXTime();

		pthread_mutex_lock(&PhysicsEngineMutex);	
		for (int i=0;i<numPhysicsTickPreCallbacks+1;i++)
		{
			C_PhysicsTickPreCallbacks[i]();
		}


		GE_TickPhysics();
		internal_physics_object_gc_cycle();

		for (int i=0;i<numPhysicsTickDoneCallbacks+1;i++)
		{
			C_PhysicsTickDoneCallbacks[i]();
		}



		pthread_mutex_unlock(&PhysicsEngineMutex);




		//SDL_Delay(1);
		//wait the rest of the 16.6ms
		finishTime = GE_GetUNIXTime();
		double sleepTime = fmax((PhysicsDelaySeconds-(finishTime-startTime))*10e4 ,0);
		//printf("delay %f\n",sleepTime);
		//printf("delaysecod %f\n",PhysicsDelaySeconds);
		//printf("%f\n",finishTime-startTime);

		/*printf("delta time %f\n",(finishTime-startTime));
		printf("sleep time %f\n",sleepTime);
		printf("finish time %f\n",finishTime);*/


		usleep(sleepTime); //delay the physics engine thread to get a perfect 16.67ms/tick

		//10e4 is derrived from 10e6 (to take seconds and microseconds and produce seconds with decimals, eg. 1.656785) 
	

#ifdef PHYSICS_DEBUG_SLOWRENDERS
		}
#endif

		

	}
	printf("Physics Engine thread is shutting down.\n");
	PhysicsEngineThreadShutdown.store(true);
	pthread_exit(static_cast<void*>(NULL));
}

void GE_TickPhysics()
{
	ticknum++;
	
	for (GE_PhysicsObject* object : allPhysicsObjects)
	{
		if (checkNotDead(object))
		{
			if (!GE_TickPhysics_ForObject(object))
			{
				if(object->callCallbackUpdate && GE_PhysicsEngine_TickingObjectsEnabled)
				{
					object->C_Update();
				}
			}
		}
		//printf("x %d y %d\n",allPhysicsObjects[i]->position.x,allPhysicsObjects[i]->position.y);
	}
#ifdef PHYSICS_DEBUG_SLOWRENDERS	
	localdebug_worldsectors();
#endif

}

struct InternalResult
{
	bool deleteMe;
	bool didCollide;
};
InternalResult GE_CollisionFullCheck(GE_PhysicsObject* cObj, GE_PhysicsObject* victimObj)
{
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
				double ABAB = GE_Dot(AB,AB);
				double ADAD = GE_Dot(AD,AD);

				//TODO: This doesn't work because the points might not intersect but the lines could.
				check = ( ( 0.0 <= AMAB ) && ( AMAB <= ABAB ) && ( 0.0 <= AMAD ) && ( AMAD <= ADAD ) ) ;


				

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
				
					cObj->position = cObj->lastGoodPosition;

					victimObj->position = victimObj->lastGoodPosition;



					//TODO: Add the inverse of the speed given to the other object in order to have conservation of energy (specifically: not duplicating the energy transfered).

					Vector2r cObjVelocity = cObj->velocity;
					
					Vector2r momentum = ((cObj->velocity*cObj->mass)-(victimObj->velocity*victimObj->mass));
					
					printf("momentum %f,%f\n",momentum.x,momentum.y);

					GE_InelasticCollision(cObj,myPoints[me],momentum,false);
					
					//newVelocity = cObjVelocity*cObj->mass;//GE_InelasticCollisionVelocityExchange(cObj->velocity,victimObj->velocity,cObj->mass,victimObj->mass);
					momentum = ((cObjVelocity*cObj->mass)/victimObj->mass);
					
					GE_InelasticCollision(victimObj,myPoints[me],momentum,true);







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

	physics_area_single_coord_t xMax = std::ceil<int>((cObj->position.x+cObj->grid.x)/PHYSICS_AREA_SIZE);
	physics_area_single_coord_t yMax = std::ceil<int>((cObj->position.y+cObj->grid.x)/PHYSICS_AREA_SIZE);


	for(physics_area_single_coord_t x=xMin;x<=xMax;x++)
	{
		for (physics_area_single_coord_t y=yMin;y<=yMax;y++)
		{
#ifdef PHYSICS_DEBUG_SLOWRENDERS
			GE_DEBUG_TextAt_PhysicsPosition(std::to_string(x)+", y "+std::to_string(y)+"( x "+std::to_string(cObj->position.x)+" y "+std::to_string(cObj->position.y)+")",cObj->position);
#endif
			physics_area_coord_t coord = physics_area_coord_t(x,y);
			auto area_it = world.find(coord);
			physics_area_t* area;
			if (area_it == world.end())
			{
				world[coord]  = new physics_area_t();
				area = world[coord];
				//printf("New map spot!\n");
			}
			else
			{
				area = area_it->second;
			}
			

			area->push_front(cObj);

			cObj->areas.insert(cObj->areas.begin(),area);
		}
	}


	

	if (GE_PhysicsEngine_CollisionsEnabled)
	{
		for (physics_area_t* area : cObj->areas)
		{

			for (auto it = area->begin(); it != area->end();it++)
			{
				GE_PhysicsObject* victimObj = *it;
				if((victimObj != cObj) && checkNotDead(victimObj))
				{
					double maxSize = cObj->radius;//fmax(cObj->grid.x, cObj->grid.x); //fmax tested to be about 2x faster than std::max in this situation
					double theirMaxSize = victimObj->radius;//fmax(victimObj->grid.x, victimObj->grid.x);
					//take a sphere arround both objects which represents the greatest possible size they could take up. do they collide?
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
	}
	//printf("TOTAL CHECKS: %d\n",temp3);
	
	cObj->lastGoodPosition = cObj->position;
	return {false,false};
}
bool GE_TickPhysics_ForObject(GE_PhysicsObject* cObj)
{
	//To avoid Clipping/"Bullet through paper" effect, we will slow down the physics simulation for a specific object by an unsigned nonzero integer and then slow down velocity accordingly. The user SHOULD NOT be able to tell any of this is happening. For performance, velocity is passed as a pointer to the internal function.
	
	unsigned int miniTickrate = fmax(ceil(  ((abs(cObj->velocity.x)+abs(cObj->velocity.y))/PHYSICS_MAX_SPEED_BEFORE_BROKEN_INTO_MINI_TICKS ) ),1.0); //minimum of 1. //TODO adjust this to good value
	miniTickrate = std::min(miniTickrate,static_cast<unsigned int>(256));
	//printf("minitickrate %d\n",miniTickrate);
	
	
	Vector2r* velocity = new Vector2r{cObj->velocity.x/miniTickrate, cObj->velocity.y/miniTickrate, cObj->velocity.r/miniTickrate};

	//printf("MINITICKRATE: %d\n,",miniTickrate);




	for (unsigned int i = 0; i <= miniTickrate; i++)
	{//TODO: Because of this mini-tick system, objects that collide with a i/miniTickrate value of <1 will have non-up-to-date velocities being added. 
		InternalResult result = GE_TickPhysics_ForObject_Internal(cObj,velocity); 
		if (result.deleteMe)
		{
			return true;
		}
		else if (result.didCollide)
		{
			//break;
			velocity = new Vector2r{cObj->velocity.x/miniTickrate, cObj->velocity.y/miniTickrate, cObj->velocity.r/miniTickrate}; //TODO: Velocity could theoreticaly increase by 1,000 times then clip through something. recalculate mini-tickrate
		}
	}

#ifdef PHYSICS_DEBUG_SLOWRENDERS
	DEBUG_isCObj = true;
	for (int a=0;a<=cObj->numCollisionRectangles;a++)
	{
		Vector2 myPoints[4] = {};
		GE_RectangleToPoints(cObj->collisionRectangles[a],cObj->grid,myPoints,cObj->position);
	}
#endif

	delete velocity;
	return false;

}



void GE_FreePhysicsObject(GE_PhysicsObject* physicsObject)
{
	for (int i=0;i<=physicsObject->numGlueTargets;i++)
	{
		GE_FreeGlueObject(physicsObject->glueTargets[i]);
		physicsObject->glueTargets[i] = NULL;
	}
	deadPhysicsObjects.insert(physicsObject);
	physicsObject->C_Destroyed();
}



void GE_RectangleToPoints(GE_Rectangler rect, Vector2 grid, Vector2* points, Vector2r hostPosition) 
{
		
	//Points make collision checking easy because you cannot rotate a point in space (or rather, rotating it would have no effect)

	points[0] = {rect.x , rect.y}; //top left
	points[1] = {rect.x+rect.w , rect.y}; //top right
	points[2] = {rect.x , rect.y+rect.h}; //bottom left
	points[3] = {rect.x+rect.w , rect.y+rect.h}; //bottom right

	for (int i =0; i < 4; i++)
	{
		points[i].x -= grid.x/2;
		points[i].y -= grid.y/2;
		GE_Vector2RotationCCW(&points[i],hostPosition.r+rect.r);
		points[i].x += /*halfrectw+*/hostPosition.x;
		points[i].y += /*halfrecth+*/hostPosition.y;


		#ifdef PHYSICS_DEBUG_SLOWRENDERS

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
		#endif

	}

}
bool GE_IsPointInPhysicsObject(Vector2 point, GE_PhysicsObject* obj)
{
	Vector2 rectPoints[4];
	for (int i=0;i!=obj->numCollisionRectangles;i++)
	{
		
		GE_RectangleToPoints(obj->collisionRectangles[i],obj->grid,rectPoints,obj->position);

		Vector2 AM = point-rectPoints[0]; //M-A
		Vector2 AB = rectPoints[1]-rectPoints[0];
		Vector2 AD = rectPoints[2]-rectPoints[0]; //bottom-left minus top left

		double AMAB = GE_Dot(AM,AB);
		double AMAD = GE_Dot(AM,AD);
		double ABAB = GE_Dot(AB,AB);
		double ADAD = GE_Dot(AD,AD);

		if( ( ( 0.0 <= AMAB ) && ( AMAB <= ABAB ) && ( 0.0 <= AMAD ) && ( AMAD <= ADAD ) ) )
		{
			return true;
		}
		printf("physics.cppfailcheck \n");
	}
	return false;
}





void GE_InelasticCollision(GE_PhysicsObject* subject, Vector2 collisionPoint, Vector2r momentum, bool CCW) 
{
	//newVelocity = newVelocity - subject->velocity;

	//find the point of collision
	
	//Vector2 collisionPoint = myPoints[me];

	printf("collisionPoint %f,%f\n",collisionPoint.x,collisionPoint.y);


	//find the angle to apply the velocity at
	
	//Vector2 centerOfMass = (subject->grid/2)+subject->position; //NOTE: Changed
	

	Vector2 centerOfMass = {subject->position.x+subject->centerOfMass.x,subject->position.y+subject->centerOfMass.y}; //TODO temporarily put the com at the centroid

	double adjacent = std::cos(collisionPoint.x-centerOfMass.x)/(GE_Distance(collisionPoint,centerOfMass));


	//adjacent *= RAD_TO_DEG;

	printf("adjacent %f\n (appx %f deg)\n",adjacent,adjacent*57.295779524);

	//subject->velocity = Vector2r{0,0,0};
	if (!CCW) adjacent += M_PI; //TODO temp
	subject->velocity.addRelativeVelocity(Vector2r{momentum.x/subject->mass,momentum.y/subject->mass,adjacent});
	
	printf("new velocity real %f, %f, %f\n",subject->velocity.x,subject->velocity.y,subject->velocity.r);
}


std::set<GE_PhysicsObject*> GE_GetObjectsInRadius(Vector2 position, double radius)
{
	std::set<GE_PhysicsObject*> objects;
	//search the map, searching through each sector that could possibly contain an object with a distance less than "radius" to "position"
	for (physics_area_single_coord_t x = std::floor<physics_area_single_coord_t >((position.x-radius)/PHYSICS_AREA_SIZE); x<= std::ceil<physics_area_single_coord_t >((position.x+radius)/PHYSICS_AREA_SIZE);x++)
	{
		for (physics_area_single_coord_t y = std::floor<physics_area_single_coord_t >((position.y-radius)/PHYSICS_AREA_SIZE); y<= std::ceil<physics_area_single_coord_t >((position.y+radius)/PHYSICS_AREA_SIZE);y++)
		{
			physics_area_coord_t coord = physics_area_coord_t(x,y);
			auto area_it = world.find(coord);
			if (area_it != world.end())
			{
				physics_area_t* area = area_it->second;
				for (GE_PhysicsObject* obj : *area)
				{
					if (GE_Distance(position,obj->position) <= radius)
					{
						objects.insert(obj);
					}
				}
			}
		}
	}
	return objects;
}


void GE_ResetPhysicsEngine()
{
	pthread_mutex_lock(&PhysicsEngineMutex);
	pthread_mutex_lock(&GlueMutex);
	GE_GlueSetSafeMode(false);

	for (GE_PhysicsObject* object : allPhysicsObjects)
	{
		GE_FreePhysicsObject(object);
	}
	ticknum = 0;
	GE_GlueSetSafeMode(true);
	pthread_mutex_unlock(&GlueMutex);
	pthread_mutex_unlock(&PhysicsEngineMutex);
}

void GE_ShutdownPhysicsEngine()
{
	GE_ResetPhysicsEngine();
	pthread_join(PhysicsEngineThread,NULL);
}
