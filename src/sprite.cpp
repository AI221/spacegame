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
GE_Sprite* Sprites[MAX_SPRITES_LOADED];
std::string Sprite_Names[MAX_SPRITES_LOADED];
int countSprites = -1;



/*!
 * This is an embedded monocolor BMP image, used in place of a missing file. This needs to be embedded, because it HAS to be available. 
 */
unsigned char MissingImage_HEX[] = {66,77,82,0,0,0,0,0,0,0,62,0,0,0,40,0,0,0,11,0,0,0,5,0,0,0,1,0,1,0,0,0,0,0,20,0,0,0,196,14,0,0,196,14,0,0,2,0,0,0,2,0,0,0,255,255,255,0,0,0,255,0,23,96,0,0,119,96,0,0,53,64,182,0,113,10,0,0,23,118,226,219};
//const char MissingImage_HEX[] = {0x4d, 0x42, 0x00, 0x52, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x000, 0x0, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x0e, 0xc4, 0x00, 0x00, 0x0e, 0xc4, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x60, 0x17, 0x00, 0x00, 0x60, 0x77, 0x00, 0x00, 0x40, 0x35, 0x00, 0xb6, 0x0a, 0x71, 0x00, 0x00, 0x6e, 0x17, 0x60, 0xcf, 0x00}; //can you imagine doing this without vim
//
//const char MissingImage_HEX[] = "BMR       >   (                  Ä  Ä        ÿÿÿ   ÿ `  w`  5@¶ q\n  nÏ`";

/*!
 * Call before using the Sprite subsystem. Do not use the sprite subsystem without initializing it first.
 * 
 * @return An error code, or 0 if it was okay. DO NOT use the sprite subsystem if there was an error code.
 *
 * Interally, this loads the MissingImage sprite, from memory. 
 */
int GE_SpriteInit(SDL_Renderer* renderer)
{
	SDL_RWops* MissingImage_RWops = SDL_RWFromMem(MissingImage_HEX, sizeof(MissingImage_HEX));
	//load that image
	SDL_Texture* spriteTexture;
	int error = GE_BMPPathToImg(&spriteTexture,renderer,MissingImage_RWops);
	if (error > 0)
	{
		return error;
	}



	countSprites++;

	Sprites[0] = GE_CreateSprite(renderer, spriteTexture);
	Sprite_Names[0] = "ERR";


	return 0;

}


GE_Sprite* GE_CreateSprite(SDL_Renderer* renderer, SDL_Texture* spriteTexture)
{
	return new GE_Sprite{renderer,spriteTexture};
}
GE_Sprite* GE_CreateSprite(SDL_Renderer* renderer, std::string path)
{
	SDL_Texture* spriteTexture;
	int error = GE_BMPPathToImg(&spriteTexture, renderer,path);
	if (error > 0)
	{
		
		return NULL;
	}


	return GE_CreateSprite(renderer,spriteTexture);
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
	return 0; //return the id of ERR
}


void GE_BlitSprite(GE_Sprite* sprite, Vector2r position,Vector2 size, GE_Rectangle animation, GE_Flip flip)	
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
		case GE_FLIP_NONE:
			flip_real = SDL_FLIP_NONE;
			break;
		case GE_FLIP_HORIZONTAL:
			flip_real = SDL_FLIP_HORIZONTAL;
			break;
		case GE_FLIP_VERTICAL:
			flip_real = SDL_FLIP_VERTICAL;
			break;
		case GE_FLIP_DIAGONAL:
			flip_real = (SDL_RendererFlip) ((SDL_RendererFlip) SDL_FLIP_HORIZONTAL | (SDL_RendererFlip) SDL_FLIP_VERTICAL);
			break;
	}

	GE_BlitSprite(sprite,renderPosition,renderAnimation,position.r,flip_real);
}
void GE_BlitSprite(GE_Sprite* sprite, SDL_Rect renderPosition, SDL_Rect renderAnimation,double rotation, SDL_RendererFlip flip) //not super recommended to invoke this function directly
{
		
	SDL_RenderCopyEx(sprite->renderer, sprite->texture, &renderAnimation, &renderPosition,rotation,NULL,flip); //RenderCopyEx copies the data from the pointers to the SDL_Rects, meaning they can be discarded immediatly following the function call.
}

void GE_FreeSprite(GE_Sprite* sprite) //sprite MUST be allocated with new. 
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

/*!
 * Gives you a SDL_Texture from a path and renderer
 * 
 * @param result The SDL_Texture the result will be put into
 * @param renderer The renderer to use for this
 * @param path The path to grab the BMP image from, relative or absolute.
 */
int GE_BMPPathToImg(SDL_Texture** result, SDL_Renderer* renderer, std::string path)
{
	return GE_BMPPathToImg(result, renderer, SDL_RWFromFile(path.c_str(),"rb"));
}

/*!
 * Same as other overload.
 * @param data Will be deleted
 */
int GE_BMPPathToImg(SDL_Texture** result, SDL_Renderer* renderer, SDL_RWops* data)
{
	SDL_Surface* LoadingSurface;
	LoadingSurface = SDL_LoadBMP_RW(data,1);
	if (LoadingSurface == NULL)
	{
		SDL_FreeSurface(LoadingSurface);
		return 1; 
	}
	*result = SDL_CreateTextureFromSurface(renderer, LoadingSurface);
	SDL_FreeSurface(LoadingSurface);
	if (result == NULL)
	{
		return 2;
	}
	return 0;

	
}
