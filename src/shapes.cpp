#include "shapes.h"


void GE_Shape::render(Vector2r position, Vector2 size) {}

GE_RectangleShape::GE_RectangleShape(SDL_Renderer* renderer, GE_Color color)
{
	this->renderer = renderer;
	SDL_Surface* LoadingSurface;

	LoadingSurface = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 0, 0);
	SDL_FillRect(LoadingSurface, NULL, SDL_MapRGBA(LoadingSurface->format, color.r,color.g,color.b,color.a));

	colorTexture = SDL_CreateTextureFromSurface(renderer, LoadingSurface);

	SDL_FreeSurface(LoadingSurface);
}
GE_RectangleShape::~GE_RectangleShape()
{
	SDL_DestroyTexture(colorTexture);
}
SDL_Rect renderAnimation = {0,0,1,1}; //this is constant for all rectangles
void GE_RectangleShape::render(Vector2r position, Vector2 size)
{
	SDL_Rect renderPosition = {static_cast<int>(position.x+.5),static_cast<int>(position.y+.5),static_cast<int>(size.x+.5),static_cast<int>(size.y+.5)};
	SDL_RenderCopyEx(this->renderer, this->colorTexture, &renderAnimation, &renderPosition,position.r,NULL,SDL_FLIP_NONE); //RenderCopyEx copies the data from the pointers to the SDL_Rects, meaning they can be discarded immediatly following the function call.
}
void GE_RectangleShape::render(Vector2 position, Vector2 size)
{
	render({position.x,position.y,0},size);
}
