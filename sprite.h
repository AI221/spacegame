#include "vector2.h"
#include<SDL2/SDL.h>

#ifndef __SPRITE_INCLUDED
#define __SPRITE_INCLUDED
class Sprite
{
	public:
		void blit(Vector2r position,Vector2 animation);
		void blit(Vector2 position,Vector2 animation, double rotation);
		void blit(Vector2 position,Vector2 animation);
		void blit(Vector2 position);
		void setAnimation(int newAnimationNumber());
		Sprite(SDL_Renderer* renderer, const char* path, Vector2 size);

	private:
		SDL_Texture* myTexture;
		SDL_Renderer* myRenderer;
		SDL_Rect myPosition; //this object's x and y are NOT GAURUNTEED and should be assumed random.
		SDL_Rect myAnimation;
};
#endif  //__SPRITE_INCLUDED
