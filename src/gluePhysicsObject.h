/*!
 * @file
 * @author Jackson McNeill
 *
 * 'Glues' one value to another, memcpy'ing it during the specified event (physics tick, render frame, etc.). Watch for undefined behaviors with this one.
 */
#pragma once

#include <stdlib.h>
#include <pthread.h>



enum GE_PULL_ON
{
	GE_PULL_ON_PHYSICS_TICK,
	GE_PULL_ON_RENDER,
};
	

struct GE_GlueTarget;


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
 * Adds values to be set either after a physics tick, or a render cycle. updateData will be CHANGED to the value of pullData. updateData MUST be at LEAST the size of sizeOfPullData or undefined behaviour. sizeOfPullData must be NO GREATER THAN the size of pull data or undefined behaviour.
 * Generally, make sure updataData and pullData are the same type, and make the last argument sizeof(yourPullData)
 * You MUST delete this glue object BEFORE deleting updateData or pullData. Failing to do so results in undefined behaviour, most likely a crash.
 * pullData MUST be initalized or this results in undefined behaviour
 */
GE_GlueTarget* GE_addGlueSubject(void* updateData, void* pullData, GE_PULL_ON pullOn, size_t sizeOfPullData);


/*!
 * This is a callback to be called before a render. It transfers the contents of the positionBuffer to the subjects.
 */
void GE_GlueRenderCallback();

/*!
 * Frees a GlueObject.
 * You MUST delete glue objects BEFORE deleting updateData or pullData. Failing to do so results in undefined behaviour, most likely a crash.
 */
void GE_FreeGlueObject(GE_GlueTarget* subject);




/*!
 * Weather GE_FreeGlueObject should lock the glue mutex, or wheather you will do it yourself.
 */
void GE_GlueSetSafeMode(bool _safe);
