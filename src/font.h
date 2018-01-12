/*!
 * @file
 * @author Jackson McNeill
 *
 * A font loading system
 */

#include <SDL2/SDL_ttf.h>
#include <string>
#include <map>

//Local includes
#include "json.h"
#include "FS.h"



#ifndef __FONT_INCLUDED
#define __FONT_INCLUDED

typedef TTF_Font* GE_Font;



int GE_Font_Init();

void GE_Font_LoadFromList(std::string list);
GE_Font GE_Font_GetFont(std::string name, unsigned int size);
void GE_Font_Shutdown();


#endif // __FONT_INCLUDED
