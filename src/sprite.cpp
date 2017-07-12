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


Sprite* GE_CreateSprite(SDL_Renderer* renderer, std::string path, int w, int h)
{
	return new Sprite{renderer,GE_PathToImg(renderer,path),w,h};
}



int GE_LoadSpritesFromDir(SDL_Renderer* renderer, std::string directory)
{
	DirList list = GE_ListInDir(directory);
	switch (list.error)
	{
		case 1:
			return 1;
			break;
	};
	for (int i = 0; i < list.list.size(); i++)
	{
		GE_LoadSpriteFromPath(renderer, list.list[i]);	
	}
	return 0;

}

int GE_LoadSpriteFromPath(SDL_Renderer* renderer, std::string path)
{
	countSprites++;
	Sprites[countSprites] = GE_CreateSprite(renderer, path, 25, 25); //TODO: There needs to be a seperate file describing properties of the sprite
	Sprite_Names[countSprites] = path; //sprite names are relative paths to simplify things
}
int GE_SpriteNameToID(std::string name)
{
	for (int i = 0; i < countSprites; i++)
	{
		if (Sprite_Names[i] == name)
		{
			return i;
		}
	}
	return -1;
}


void GE_BlitSprite(Sprite* sprite, Vector2r position,Vector2 animation)	
{
	GE_BlitSprite(sprite,{position.x,position.y},animation,position.r);
}
void GE_BlitSprite(Sprite* sprite, Vector2 position,Vector2 animation,double rotation)	
{
	SDL_Rect renderPosition = {0,0,25,25};
	SDL_Rect renderAnimation = {0,0,0,0};
	renderPosition.x = position.x;
	renderPosition.y = position.y;
	renderPosition.w = sprite->w;
	renderPosition.h = sprite->h;
	renderAnimation.x = animation.x;
	renderAnimation.y = animation.y;
	renderAnimation.w = sprite->w;
	renderAnimation.h = sprite->h;
	
	SDL_RenderCopyEx(sprite->renderer, sprite->texture, &renderAnimation, &renderPosition,rotation,NULL,SDL_FLIP_NONE);
}

void GE_FreeSprite(Sprite* sprite) //sprite MUST be allocated with new. 
{
	SDL_DestroyTexture(sprite->texture);
	delete sprite;
}

SDL_Texture* GE_PathToImg(SDL_Renderer* renderer, std::string path)
{
	SDL_Surface* LoadingSurface;
	LoadingSurface = SDL_LoadBMP(path.c_str());
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, LoadingSurface);	
	SDL_FreeSurface(LoadingSurface);
	return texture;
}
