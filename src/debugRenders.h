/*!
 * @file
 * @author Jackson McNeill
 *
 * Debug-grade renderers for debugging. 
 */

#ifdef DEBUG_RENDERS
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
#include "vector2.h"
#include "debugUI.h"
#include "sprite.h"
#include "camera.h"
#include "physics.h"
#include "renderedObject.h"
#include "UI.h"

#define SPRITE_DIR "../sprites/"


//TODO: Merge physicsEngine's render pass to here, and have it use the renderer given here. 

#ifndef __DEBUGRENDERS_INCLUDED
#define __DEBUGRENDERS_INCLUDED

/*!
 *
 * Shows the sGrid at the given camera position. Does not factor in rotation.
 *
 */
void Debug_sGrid_Render(Vector2r camerapos);




#endif // __DEBUGRENDERS_INCLUDED
#endif //DEBUG_RENDERS
