/*
Copyright 2017 Jackson Reed McNeill

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*!
 *	@file
 *	@author Jackson McNeill
 *
 *	A sprite system
 *
 */


#include "SDL.h"
#include "SDL_image.h"
#include <string>
#include <vector>


#include "vector2.h"
#include "FS.h"
#include "GeneralEngineCPP.h"

#ifndef __SPRITE_INCLUDED
#define __SPRITE_INCLUDED

//LIMITS

#define MAX_SPRITES_LOADED 1024


/*!
 * Used to tell how your sprite image is flipped
 */
enum GE_Flip
{
	GE_FLIP_NONE,
	GE_FLIP_HORIZONTAL,
	GE_FLIP_VERTICAL,
	GE_FLIP_DIAGONAL
};

/*!
 * Holds data for a sprite
 */
struct GE_Sprite
{
	SDL_Renderer* renderer;
	SDL_Texture* texture;
};

/*!
 * List of loaded sprites
 */
extern GE_Sprite* Sprites[MAX_SPRITES_LOADED];
extern std::string Sprite_Names[MAX_SPRITES_LOADED]; //basically, all things should pass the name(/relative directory) of a sprite, for it to be looked up in here to find the number associated with it. However, the resulting number should be stored rather than the path, for effeciency.
extern int countSprites;

/*!
 * Call before using the Sprite subsystem. Do not use the sprite subsystem without initializing it first.
 * 
 * @return An error code, or 0 if it was okay. DO NOT use the sprite subsystem if there was an error code.
 *
 * Interally, this loads the MissingImage sprite, from memory. 
 */
int GE_SpriteInit(SDL_Renderer* renderer);

/*!
 * Loads all sprites from a directory into Sprites. Do not place anything but compatible image types (.bmp, .png , etc.) in this folder. This will not recursively search directories.
 */
int GE_LoadSpritesFromDir(SDL_Renderer* renderer, std::string directory);
/*!
 * Loads a sprite from a path and puts it into Sprites
 */
int GE_LoadSpriteFromPath(SDL_Renderer* renderer, std::string path);

/*!
 * Gives the the ID to a sprite from its full path ("name")
 */
int GE_SpriteNameToID(std::string name);

/*!
 * Recommended function to use to blit a sprite
 */
void GE_BlitSprite(GE_Sprite* sprite, Vector2r position,Vector2 size, GE_Rectangle  animation, GE_Flip flip);

/*!
 * Internal overload--subject to future change, don't use
 */
void GE_BlitSprite(GE_Sprite* sprite, SDL_Rect renderPosition, SDL_Rect renderAnimation,double rotation, SDL_RendererFlip flip); 



/*!
 * Internal function to create a sprite using a texture
 */
GE_Sprite* GE_CreateSprite(SDL_Renderer* renderer, SDL_Texture* spriteTexture);

/*!
 * Internal function to create a sprite using a path to an image file
 */
GE_Sprite* GE_CreateSprite(SDL_Renderer* renderer, std::string path); //should not be used by others *

/*!
 * Converts an SDL_RWops of a .bmp , .png , etc. to a SDL_Texture 
 */
int GE_BMPPathToImg(SDL_Texture** result, SDL_Renderer* renderer, SDL_RWops * data);

/*!
 * Converts an image file (.bmp , .png , etc.) to an SDL_Texture
 */
int GE_BMPPathToImg(SDL_Texture** result, SDL_Renderer* renderer, std::string path);

/*!
 * Deletes a GE_Sprite that was allocated with new (GE_CreateSprite allocates with new)
 */
void GE_FreeSprite(GE_Sprite* sprite); 

/*!
 * Frees all loaded sprites in the Sprite table (This is to be called on game shutdown)
 */
void GE_FreeAllSprites();
#endif
