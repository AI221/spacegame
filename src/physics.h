/*!
 *@file
 *@author Jackson McNeill
 *
 * An overkill 2D physics engine. Before checking "full" collision, there is a preliminatory check which involves sGrid.
 *
 * This preliminatory check works by setting elements in the 2D sGrid array to their physicsID. When overlap in this is detected, a full collision check happens. TODO: better explanation 
 */
#pragma once


#include <pthread.h>
#include <forward_list>
#include <list>
#include <unordered_map>
#include <functional>
#include <vector>

#include "vector2.h"
#include "gluePhysicsObject.h"

//#define PHYSICS_DEBUG_SLOWRENDERS 

#ifdef PHYSICS_DEBUG_SLOWRENDERS
	#include<SDL2/SDL.h>
	#include "camera.h"
	#include "renderedObject.h"
	#include "debugRenders.h"
#endif


	
	
extern bool DEBUG_allowPhysicsTick;

//LIMITS:
#define MAX_COLLISION_RECTANGLES_PER_OBJECT 32 //TODO: test if this is a good limit in practice
#define MAX_GLUE_OBJECTS_PER_OBJECT 32 
#define MAX_PHYSICS_ENGINE_DONE_CALLBACKS 64
#define MAX_PHYSICS_ENGINE_PRE_CALLBACKS 64


//CONFIGS

//Optimization
#define PHYSICS_AREA_SIZE 500
#define PHYSICS_MAX_SPEED_BEFORE_BROKEN_INTO_MINI_TICKS 20 //e.g if set to 20, an object moving at 100 units/tick would be broken up into 5 ticks of 20 units/tick

//RUNTIME CONFIG

extern double PhysicsDelaySeconds;


extern bool PhysicsEngineThreadShutdown;




class GE_PhysicsObject;


typedef long long int physics_area_single_coord_t;
typedef std::pair<physics_area_single_coord_t,physics_area_single_coord_t> physics_area_coord_t;
typedef std::forward_list<GE_PhysicsObject*> physics_area_t;
typedef std::vector<physics_area_t*> physics_object_area_list_t;


/*!
 * The basic Physics object structure. It's recommended that your game objects inhereit from this, though you can do non-OO design alternatively using glueObject's buffering.
 */
class GE_PhysicsObject
{
	public:
		GE_PhysicsObject(Vector2r position, Vector2r velocity, double mass);
		virtual ~GE_PhysicsObject();




		Vector2r position;
		Vector2r velocity;
		double mass;
		Vector2 warpedShape;
		int ID;
		GE_Rectangle collisionRectangles[MAX_COLLISION_RECTANGLES_PER_OBJECT];
		int numCollisionRectangles;
		Vector2 grid; //the bounding box is the smallest box that can fit all collision rectangles
		GE_GlueTarget* glueTargets[MAX_GLUE_OBJECTS_PER_OBJECT]; //Hold glue targets to delete them right before we're deleted //TODO this forces you to keep your glue active until you kill the physics object. This is undesired.
		int numGlueTargets;
		Vector2r lastGoodPosition;
		bool callCallbackBeforeCollisionFunction;
		bool callCallbackAfterCollisionFunction;

		/*!
		 * Adds a collision rectangle to this object and updates cached values
		 */
		void addCollisionRectangle(GE_Rectangle newRectangle);

		/*!
		 * A callback upon collision
		 * @param victimID the ID of the colliding physics object
		 * @param collisionRectangleID The ID of YOUR collision rectangle that was collided with 
		 * @return True if you want YOUR physics object to be deleted.
		 */
		virtual bool C_Collision(GE_PhysicsObject* victim, int collisionRectangleID);

		/*!
		 * Weather or not to call the update callback
		 */
		bool callCallbackUpdate;
		
		/*!
		 * Called during a physics tick, after position updates & collision calculations
		 */
		virtual bool C_Update();



		/*!
		 * Which area(s) the physics object is in the world (basically, a cached value you don't have to worry about)
		 */
		physics_object_area_list_t areas;
		double diameter; //the maximum space the object can take up; used for collision checking optimizations

		
		

		/*!
		 * Game-specific. Intended for specifying what type this object is. Default is 0. I'd recommend to use an enum with this.
		 */
		int type;
		


};

extern pthread_t PhysicsEngineThread;

extern pthread_mutex_t PhysicsEngineMutex;


extern std::function< void ()> C_PhysicsTickDoneCallbacks[MAX_PHYSICS_ENGINE_DONE_CALLBACKS];
extern int numPhysicsTickDoneCallbacks;

extern std::function< void ()> C_PhysicsTickPreCallbacks[MAX_PHYSICS_ENGINE_PRE_CALLBACKS];
extern int numPhysicsTickPreCallbacks;


extern int ticknum;




/*!
 * Spawns a new thread containing the physics engine. 
*/
int GE_PhysicsInit();

/*!
 * Convience function that creates a new glue object, linking link to subject 's position. It will be deleted when the physics objected is freed.
 */
void GE_LinkVectorToPhysicsObjectPosition(GE_PhysicsObject* subject, Vector2r* link);

/*!
 * Convience function that creates a new glue object, linking link to subject 's velocity. It will be deleted when the physics objected is freed.
 */
void GE_LinkVectorToPhysicsObjectVelocity(GE_PhysicsObject* subject, Vector2r* link);


/*!
 * Link an arbitrary glue object so that it's destroyed when the linked physics object is destroyed
 */
void GE_LinkGlueToPhysicsObject(GE_PhysicsObject* subject, GE_GlueTarget* glue);


/*!
 * Adds a callback which will be ran after each physics tick. Used by the engine itself for gluing things to the physics engine.
 *
 * @param callback A function that gives void output and takes no input
 */
int GE_AddPhysicsDoneCallback(std::function<void ()> callback);

/*!
 * Adds a callback which will be ran before each physics tick. Used by the engine itself for gluing things to the physics engine.
 *
 * @param callback A function that gives void output and takes no input
 */
int GE_AddPhysicsPreCallback(std::function<void ()> callback);

/*!
 * Adds a callback to an individual physics object that will be called when it collides with any other physics object. It is not gauranteed your callback will be called first, or at all if your physics object is killed.
 * @param subject The physics object to add the callback to
 * @param C_Collision A function that takes GE_PhysicsObject* 's cObj(you) and victimObj(your collision partner) and  outputs true when a either cObj or victimObj has been killed. 
 * @param callCallbackBeforeCollisionFunction True to call the callback BEFORE velocities have been exchanged, false to call it AFTER. 
 */
void GE_AddPhysicsObjectCollisionCallback(GE_PhysicsObject* subject, std::function< bool (GE_PhysicsObject* cObj, GE_PhysicsObject* victimObj)> C_Collision, bool callCallbackBeforeCollisionFunction);

/*!
 * Add velocity to a physics object. Note that only 1 additional velocity will be added per tick, so calling it 3 times will result in the call being added.
 *
 * @param physicsObject The physics object to add the velocity to
 * @param moreVelocity How much more velocity to add
 */
void GE_AddVelocity(GE_PhysicsObject* physicsObject, Vector2r moreVelocity);

/*! 
 * Add velocity to the physics object, relative to its rotation. 
 *
 * @param physicsObject The physics object to add the velocity to
 * @param moreVelocity How much more velocity to add
 */
void GE_AddRelativeVelocity(GE_PhysicsObject* physicsObject, Vector2r moreVelocity);

/*! 
 * The function made into a new thread by GE_PhysicsInit . In general: Don't touch this
 */
void* GE_physicsThreadMain(void* ); 

/*! 
 * The function called every tick. In general: Don't touch this
 */
void GE_TickPhysics();

/*!
 * The function called for every physics object, during a physics tick. In general: Don't touch this
 * @param cObj The pointer to the physics object to tick
 */ 
bool GE_TickPhysics_ForObject(GE_PhysicsObject* cObj);

/*! 
 * Frees the memory used by a physics object. 
 * @param physicsObject The physics object which will be DELETED and you can no longer use afterwards
 */
void GE_FreePhysicsObject(GE_PhysicsObject* physicsObject); //MUST be allocated with new

/*!
 * Converts a rectangle to 4 points in 2D space. A point in 2D space has no rotation, so it is described by a Vector2. Use it if you want.
 * @param rect The rectangle to be converted
 * @param points An array that can contain 4 Vector2* s. 
 * @param hostPosition a Vector2r , note the r, that the GE_Rectangle rect belongs to. It is used to be added to the positions of points, and its rotation will translate them.
 */
void GE_RectangleToPoints(GE_Rectangle rect, Vector2 grid, Vector2* points, Vector2r hostPosition);

Vector2r GE_InelasticCollisionVelocityExchange(Vector2r velocity1, Vector2r velocity2, double mass1, double mass2);


Vector2 GE_GetRectangleCenterRealPosition(GE_Rectangle rectangle, Vector2r realPosition);

void GE_InelasticCollision(GE_PhysicsObject* subject, Vector2 collisionPoint, Vector2r momentum, bool CCW);
/*!
 * Frees all physics objects in memory. Call only on shutdown.
 */
void GE_ShutdownPhysicsEngine();

/*!
 * Frees all physics objects in memory.
 */
void GE_ResetPhysicsEngine();

