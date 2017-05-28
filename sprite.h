#include "vector2.h"
#include<SDL2/SDL.h>
#include <string>

#ifndef __SPRITE_INCLUDED
#define __SPRITE_INCLUDED
struct Sprite
{
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	int w;
	int h;
};
SDL_Texture* GE_PathToImg(SDL_Renderer* renderer, std::string path);
void GE_BlitSprite(Sprite* sprite, Vector2r position,Vector2 animation);
void GE_BlitSprite(Sprite* sprite, Vector2 position,Vector2 animation, double rotation);
void GE_FreeSprite(Sprite* sprite); //sprite MUST be allocated with new. 
#endif
