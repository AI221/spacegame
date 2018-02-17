/*!
 * @file
 * @author Jackson McNeill
 *
 * A font loading system
 */
#pragma once

#include <SDL2/SDL_ttf.h>
#include <string>




typedef TTF_Font* GE_Font;



int GE_Font_Init();

void GE_Font_LoadFromList(std::string list);
GE_Font GE_Font_GetFont(std::string name, unsigned int size);
void GE_Font_Shutdown();

