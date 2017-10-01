/*!
 * @file
 * @author Jackson McNeill
 *
 * Debug-grade renderers for debugging. 
 */

//debug: include pretty much everything
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <math.h>
#include <vector>
#include <functional>
//Local includes
#include "camera.h"
#include "UI.h"


#ifndef __DEBUGRENDERS_INCLUDED
#define __DEBUGRENDERS_INCLUDED

extern SDL_Renderer* GE_DEBUG_Renderer;
extern Camera* GE_DEBUG_Camera; 
void GE_DEBUG_PassRenderer(SDL_Renderer* yourRenderer, Camera* yourCamera);

void GE_DEBUG_TextAt(std::string text, Vector2 position);
void GE_DEBUG_TextAt(std::string text, Vector2r position);

void GE_DEBUG_TextAt_PhysicsPosition(std::string text, Vector2 position);

template<class XY>
void GE_DEBUG_TextAt_PhysicsPosition(std::string text, XY position)
{
	GE_DEBUG_TextAt_PhysicsPosition(text,{position.x,position.y});
}


#endif // __DEBUGRENDERS_INCLUDED
