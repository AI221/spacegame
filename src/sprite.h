#include <SDL2/SDL.h>
#include <string>
#include <vector>


#include "vector2.h"
#include "FS.h"

#ifndef __SPRITE_INCLUDED
#define __SPRITE_INCLUDED

//LIMITS

#define MAX_SPRITES_LOADED 256


struct Sprite
{
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	int w;
	int h;
};

extern Sprite* Sprites[MAX_SPRITES_LOADED];
extern std::string Sprite_Names[MAX_SPRITES_LOADED]; //basically, all things should pass the name(/relative directory) of a sprite, for it to be looked up in here to find the number associated with it. However, the resulting number should be stored rather than the path, for effeciency.
extern int countSprites;

int GE_LoadSpritesFromDir(SDL_Renderer* renderer, std::string directory);
int GE_LoadSpriteFromPath(SDL_Renderer* renderer, std::string path);
int GE_SpriteNameToID(std::string name);
void GE_BlitSprite(Sprite* sprite, Vector2r position,Vector2 animation);
void GE_BlitSprite(Sprite* sprite, Vector2 position,Vector2 animation, double rotation);


Sprite* GE_CreateSprite(SDL_Renderer* renderer, std::string path, int w, int h); //should not be used by others *
SDL_Texture* GE_PathToImg(SDL_Renderer* renderer, std::string path);//should not be used by others *
void GE_FreeSprite(Sprite* sprite); //sprite MUST be allocated with new. //should not be used by others *
//* but there may be cases I'm not thinking of where it would be, e.x. client-sided mod.
#endif
