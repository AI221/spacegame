/*!
 * @file
 * @author Jackson McNeill
 *
 * Rendering shapes that are abstracted over SDL's, offering additional features
 */
#pragma once


#include <SDL2/SDL.h>

//Local incluides
#include "vector2.h"



struct GE_Color
{
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
	operator SDL_Color()
	{
		return SDL_Color{r,g,b,a};
	}
};
class GE_Shape
{
	public:
		virtual void render(Vector2r position, Vector2 size);
};



class GE_RectangleShape : public GE_Shape
{
	public:
		GE_RectangleShape(SDL_Renderer* renderer, GE_Color color);
		~GE_RectangleShape();
		void render(Vector2r position, Vector2 size);
		void render(Vector2 position, Vector2 size);
	private:
		SDL_Renderer* renderer;
		SDL_Texture* colorTexture;

};

