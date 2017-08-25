/*!
 * @file
 * @author Jackson McNeill
 *
 * 'Glues' the position of a physics object to a Vector2r pointer, updating it each physics tick
 */
#include <pthread.h>

#include "stdio.h"
#include "physics.h"
#include "vector2.h"
#include "renderedObject.h"
#include "GeneralEngineCPP.h"

//LIMITS

#define MAX_GLUE_TARGETS 1024

#ifndef __GLUE_PHYSICS_OBJECT_INCLUDED
#define __GLUE_PHYSICS_OBJECT_INCLUDED

enum GE_ADD_TYPE
{
	GE_ADD_TYPE_NONE = 0,
	GE_ADD_TYPE_NORM = 1,
	GE_ADD_TYPE_RELATIVE = 2,
};
	

struct GE_GlueTarget
{
	Vector2r* subject;
	int physicsObjectID; 

	Vector2r addVelocity;
	GE_ADD_TYPE typeAddVelocity;
	Vector2r addPosition;
	GE_ADD_TYPE typeAddPosition;
};

extern GE_GlueTarget targets[MAX_GLUE_TARGETS];
extern Vector2r positionBuffer[MAX_GLUE_TARGETS];
extern Vector2r velocityBuffer[MAX_GLUE_TARGETS];
extern GE_Rectangle gridbuffer[MAX_GLUE_TARGETS];
extern int countGlueTargets;

extern pthread_mutex_t GlueMutex;
/*!
 * Adds a callback to the Physics engine to run GE_GlueCallback after every physics tick
 */
int GE_GlueInit();

/*!
 * Update add(Velocity/Position) things. Called before a physics tick/
 */
void GE_GluePreCallback();
/*!
 * Updates all glue subjects. Will lock both PhysicsEngineMutex and RenderEngineMutex . DO NOT CALL AFTER LOCKING THOSE MUTEXES IN THE SAME THREAD.
 *
 * This is an internal engine function, so generally don't call at all.
 */
void GE_GlueCallback();

/*!
 *
 * @param subject Will be updated to physicsID's position value each physics tick
 * @physicsID The ID of the physics object to 'glue' subject to.
 */
int GE_addGlueSubject(Vector2r* subject, int physicsID);

void GE_glueAddVelocity(int targetID, Vector2r ammount, GE_ADD_TYPE type);

void GE_glueAddPosition(int targetID, Vector2r ammount, GE_ADD_TYPE type);

/*!
 * This is a callback to be called before a render. It transfers the contents of the positionBuffer to the subjects.
 */
void GE_GlueRenderCallback();

#endif //__GLUE_PHYSICS_OBJECT_INCLUDED
