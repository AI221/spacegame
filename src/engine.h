/*! 
 * @file
 * @author Jackson McNeill
 *
 * General engine things that don't really belong anywhere else
 */
#include <SDL2/SDL.h>

#include "gluePhysicsObject.h"
#include "gluePhysicsObjectInit.h"
#include "physics.h"
#include "sprite.h"
#include "renderedObject.h"
#include "isOn.h"
#include "json.h"
#include "UI.h"



#ifndef __ENGINE_INLCUDED
#define __ENGINE_INLCUDED

/*!
 * Initialize the following engine components:
 * Sprite
 * Physics Engine
 * PhysicsEngine glue
 */
int GE_Init(SDL_Renderer* renderer);

/*!
 * Shut down all engine components. Do not attempt to use anything after this.
 */
void GE_Shutdown();


#endif //__ENGINE_INLCUDED
