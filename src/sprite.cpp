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
#include "sprite.h"
Sprite* Sprites[MAX_SPRITES_LOADED];
std::string Sprite_Names[MAX_SPRITES_LOADED];
int countSprites = -1;


Sprite* GE_CreateSprite(SDL_Renderer* renderer, std::string path)
{
	SDL_Texture* spriteTexture;
	int error = GE_BMPPathToImg(spriteTexture, renderer,path);

	return new Sprite{renderer,spriteTexture};
}



int GE_LoadSpritesFromDir(SDL_Renderer* renderer, std::string directory)
{
	DirList list = GE_ListInDir(directory);
	switch (list.error)
	{
		case 1:
			return 1;
			break;
		default: 
			return list.error;
			break;
	};
	for (int i = 0; i < list.list.size(); i++)
	{
		GE_LoadSpriteFromPath(renderer, directory+list.list[i]);
	}
	return 0;

}

int GE_LoadSpriteFromPath(SDL_Renderer* renderer, std::string path)
{
	countSprites++;
#ifdef outdatedOS
	Sprites[countSprites] = GE_CreateSprite(renderer, GE_ReverseSlashes(path)); 
#else
	Sprites[countSprites] = GE_CreateSprite(renderer, path); //TODO: There needs to be a seperate file describing properties of the sprite
#endif
	Sprite_Names[countSprites] = path; //sprite names are relative paths to simplify things

	return 0;
}
int GE_SpriteNameToID(std::string name)
{
	for (int i = 0; i < countSprites+1; i++)
	{
		if (Sprite_Names[i] == name)
		{
			return i;
		}
	}
	return -1;
}


void GE_BlitSprite(Sprite* sprite, Vector2r position,Vector2 size, GE_Rectangle animation, GE_Flip flip)	
{
	SDL_Rect renderPosition = {};
	SDL_Rect renderAnimation = {};
	renderPosition.x = position.x;
	renderPosition.y = position.y;
	renderPosition.w = size.x;
	renderPosition.h = size.y;

	/*renderAnimation.x = animation.x;
	renderAnimation.y = animation.y;
	renderAnimation.w = size.x;
	renderAnimation.h = size.y;*/

	renderAnimation.x = animation.x;
	renderAnimation.y = animation.y;
	renderAnimation.w = animation.w;
	renderAnimation.h = animation.h;

	SDL_RendererFlip flip_real;

	switch(flip)
	{
		case FLIP_NONE:
			flip_real = SDL_FLIP_NONE;
			break;
		case FLIP_HORIZONTAL:
			flip_real = SDL_FLIP_HORIZONTAL;
			break;
		case FLIP_VERTICAL:
			flip_real = SDL_FLIP_VERTICAL;
			break;
		case FLIP_DIAGONAL:
			flip_real = (SDL_RendererFlip) ((SDL_RendererFlip) SDL_FLIP_HORIZONTAL | (SDL_RendererFlip) SDL_FLIP_VERTICAL);
			break;
	}

	GE_BlitSprite(sprite,renderPosition,renderAnimation,position.r,flip_real);
}
void GE_BlitSprite(Sprite* sprite, SDL_Rect renderPosition, SDL_Rect renderAnimation,double rotation, SDL_RendererFlip flip) //not super recommended to invoke this function directly
{
		
	SDL_RenderCopyEx(sprite->renderer, sprite->texture, &renderAnimation, &renderPosition,rotation,NULL,flip); //RenderCopyEx copies the data from the pointers to the SDL_Rects, meaning they can be discarded immediatly following the function call.
}

void GE_FreeSprite(Sprite* sprite) //sprite MUST be allocated with new. 
{
	SDL_DestroyTexture(sprite->texture);
	delete sprite;
}
void GE_FreeAllSprites()
{
	for (int i=0;i < countSprites+1;i++)
	{
		GE_FreeSprite(Sprites[i]);
	}
	
}

int GE_BMPPathToImg(SDL_Texture* result, SDL_Renderer* renderer, std::string path)
{
	SDL_Surface* LoadingSurface;
	LoadingSurface = SDL_LoadBMP(path.c_str());
	if (LoadingSurface == NULL)
	{
		SDL_FreeSurface(LoadingSurface);
		return 1; 
	}
	result = SDL_CreateTextureFromSurface(renderer, LoadingSurface);	
	SDL_FreeSurface(LoadingSurface);
	if (result == NULL)
	{
		return 2;
	}
	return 0;

	
}
