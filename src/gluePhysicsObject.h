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

//LIMITS

#define MAX_GLUE_TARGETS 1024

#ifndef __GLUE_PHYSICS_OBJECT_INCLUDED
#define __GLUE_PHYSICS_OBJECT_INCLUDED

struct GE_GlueTarget
{
	Vector2r* subject;
	int physicsObjectID; 
};

extern GE_GlueTarget targets[MAX_GLUE_TARGETS];
extern int countGlueTargets;

/*!
 * Adds a callback to the Physics engine to run GE_GlueCallback after every physics tick
 */
int GE_GlueInit();

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
void GE_addGlueSubject(Vector2r* subject, int physicsID);

#endif //__GLUE_PHYSICS_OBJECT_INCLUDED
