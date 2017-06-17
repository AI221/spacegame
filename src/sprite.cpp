#include "sprite.h"
/*Sprite::Sprite(SDL_Renderer* renderer, const char* path, Vector2 size)
{
	myRenderer = renderer;
	SDL_Surface* LoadingSurface;
	LoadingSurface = SDL_LoadBMP(path);
	myTexture = SDL_CreateTextureFromSurface(myRenderer, LoadingSurface);
	SDL_FreeSurface(LoadingSurface);
	myPosition.w = size.x;
	myPosition.h = size.y;
	myAnimation.w = size.x;
	myAnimation.h = size.y;
	
}*/
Sprite* GE_CreateSprite(SDL_Renderer* renderer, std::string path, int w, int h)
{
	return new Sprite{renderer,GE_PathToImg(renderer,path),w,h};
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
