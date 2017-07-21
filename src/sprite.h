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


#include <SDL2/SDL.h>
#include <string>
#include <vector>


#include "vector2.h"
#include "FS.h"
#include "GeneralEngineCPP.h"

#ifndef __SPRITE_INCLUDED
#define __SPRITE_INCLUDED

//LIMITS

#define MAX_SPRITES_LOADED 1024

enum Flip
{
	FLIP_NONE,
	FLIP_HORIZONTAL,
	FLIP_VERTICAL,
	FLIP_DIAGONAL
};

struct Sprite
{
	SDL_Renderer* renderer;
	SDL_Texture* texture;
};
struct Animation
{
	int x;
	int y;
	int w;
	int h;
};

extern Sprite* Sprites[MAX_SPRITES_LOADED];
extern std::string Sprite_Names[MAX_SPRITES_LOADED]; //basically, all things should pass the name(/relative directory) of a sprite, for it to be looked up in here to find the number associated with it. However, the resulting number should be stored rather than the path, for effeciency.
extern int countSprites;

int GE_LoadSpritesFromDir(SDL_Renderer* renderer, std::string directory);
int GE_LoadSpriteFromPath(SDL_Renderer* renderer, std::string path);
int GE_SpriteNameToID(std::string name);
void GE_BlitSprite(Sprite* sprite, Vector2r position,Vector2 size, Animation animation, Flip flip);
void GE_BlitSprite(Sprite* sprite, SDL_Rect renderPosition, SDL_Rect renderAnimation,double rotation, SDL_RendererFlip flip); //not super recommended to invoke this function directly




Sprite* GE_CreateSprite(SDL_Renderer* renderer, std::string path); //should not be used by others *
SDL_Texture* GE_PathToImg(SDL_Renderer* renderer, std::string path);//should not be used by others *
void GE_FreeSprite(Sprite* sprite); //sprite MUST be allocated with new. //should not be used by others *
//* but there may be cases I'm not thinking of where it would be, e.x. client-sided mod.

void GE_FreeAllSprites();
#endif
