#include "SDL.h"

//Local incluides
#include "vector2.h"


#ifndef __SHAPES_INCLUDED
#define __SHAPES_INCLUDED


struct GE_Color
{
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
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
	private:
		SDL_Renderer* renderer;
		SDL_Texture* colorTexture;

};

#endif //__SHAPES_INCLUDED
