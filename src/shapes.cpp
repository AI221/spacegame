#include "shapes.h"

#include <cmath>

//needed for rectangle to points
#include "physics.h"

void GE_Shape::render(Vector2r position, Vector2 size) {}

GE_RectangleShape::GE_RectangleShape(SDL_Renderer* renderer, GE_Color color)
{
	this->renderer = renderer;
	SDL_Surface* LoadingSurface;

	LoadingSurface = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 0, 0);
	SDL_FillRect(LoadingSurface, NULL, SDL_MapRGBA(LoadingSurface->format, color.r,color.g,color.b,color.a));

	colorTexture = SDL_CreateTextureFromSurface(renderer, LoadingSurface);


	SDL_SetTextureBlendMode(colorTexture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(colorTexture, color.a);

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
	GE_PhysicsRotationToRenderRotation(&position.r);
	SDL_RenderCopyEx(this->renderer, this->colorTexture, &renderAnimation, &renderPosition,position.r,NULL,SDL_FLIP_NONE); //RenderCopyEx copies the data from the pointers to the SDL_Rects, meaning they can be discarded immediatly following the function call.
}
void GE_RectangleShape::render(Vector2 position, Vector2 size)
{
	render({position.x,position.y,0},size);
}

GE_HollowRectangleShape::GE_HollowRectangleShape(SDL_Renderer* renderer, GE_Color color,double thickness)
{
	this->renderer = renderer;
	SDL_Surface* LoadingSurface;

	LoadingSurface = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 0, 0);
	SDL_FillRect(LoadingSurface, NULL, SDL_MapRGBA(LoadingSurface->format, color.r,color.g,color.b,color.a));

	colorTexture = SDL_CreateTextureFromSurface(renderer, LoadingSurface);


	SDL_SetTextureBlendMode(colorTexture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(colorTexture, color.a);

	SDL_FreeSurface(LoadingSurface);

	this->thickness = thickness;
}
GE_HollowRectangleShape::~GE_HollowRectangleShape()
{
	SDL_DestroyTexture(colorTexture);
}
SDL_Point center = {0,0};
void GE_HollowRectangleShape::render(Vector2r position, Vector2 size)
{
	position.x += thickness;
	position.y += thickness;
	size.x -=thickness;
	size.y -=thickness;
	SDL_Rect renderPosition;
	Vector2 points[4];
	GE_RectangleToPoints(GE_Rectangle{0,0,size.x,size.y},Vector2{0,0},points,position);
	GE_PhysicsRotationToRenderRotation(&position.r);

	renderPosition = {static_cast<int>(points[0].x+.5),static_cast<int>(points[0].y+.5),static_cast<int>(size.x+.5+thickness),static_cast<int>(thickness+.5)};
	SDL_RenderCopyEx(this->renderer, this->colorTexture, &renderAnimation, &renderPosition,90-(position.r),&center,SDL_FLIP_NONE); 
	renderPosition = {static_cast<int>(points[1].x+.5),static_cast<int>(points[1].y+.5),static_cast<int>(thickness+.5),static_cast<int>(size.y+.5)};
	SDL_RenderCopyEx(this->renderer, this->colorTexture, &renderAnimation, &renderPosition,90-(position.r),&center,SDL_FLIP_NONE); 


	renderPosition = {static_cast<int>(points[2].x+.5),static_cast<int>(points[2].y+.5),static_cast<int>(size.x+.5+thickness),static_cast<int>(thickness+.5)};
	SDL_RenderCopyEx(this->renderer, this->colorTexture, &renderAnimation, &renderPosition,(90-(position.r))-90,&center,SDL_FLIP_NONE); 
	renderPosition = {static_cast<int>(points[3].x+.5),static_cast<int>(points[3].y+.5),static_cast<int>(size.x+.5),static_cast<int>(thickness+.5)};
	SDL_RenderCopyEx(this->renderer, this->colorTexture, &renderAnimation, &renderPosition,(90-(position.r))-180,&center,SDL_FLIP_NONE); 

}
