/*! 
 * @file
 * @author Jackson McNeill
 *
 * General engine things that don't really belong anywhere else
 */

#include "gluePhysicsObject.h"
#include "physics.h"
#include "sprite.h"


#ifndef __ENGINE_INLCUDED
#define __ENGINE_INLCUDED

/*!
 * Initialize the following engine components:
 * Sprite
 * Physics Engine
 * PhysicsEngine glue
 */
int GE_Init(SDL_Renderer* renderer);


#endif //__ENGINE_INLCUDED
