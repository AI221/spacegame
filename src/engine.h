/*! 
 * @file
 * @author Jackson McNeill
 *
 * General engine things that don't really belong anywhere else
 */
#pragma once




class SDL_Renderer;




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

