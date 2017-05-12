#include "sprite.h"
Sprite::Sprite(SDL_Renderer* renderer, const char* path, Vector2 size)
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
	
}
void Sprite::blit(Vector2r position,Vector2 animation)	
{
	blit({position.x,position.y},animation,position.r);
}
void Sprite::blit(Vector2 position,Vector2 animation,double rotation)	
{
	myPosition.x = position.x;
	myPosition.y = position.y;
	myAnimation.x = animation.x;
	myAnimation.y = animation.y;
	
	SDL_RenderCopyEx(myRenderer, myTexture, &myAnimation, &myPosition,rotation,NULL,SDL_FLIP_NONE);
}
void Sprite::blit(Vector2 position, Vector2 animation)
{
	blit(position,animation,0);
}
void Sprite::blit(Vector2 position)
{
	this->blit(position,{0,0});
}

