#include "shapes.h"

#include <cmath>

//needed for rectangle to points
#include "physics.h"

void GE_Shape::render(Vector2r UNUSED(position), Vector2 UNUSED(size)) {}

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
void GE_RectangleShape::render(Vector2r position, Vector2 size,	Vector2 axis)
{
	SDL_Rect renderPosition = {static_cast<int>(position.x+.5),static_cast<int>(position.y+.5),static_cast<int>(size.x+.5),static_cast<int>(size.y+.5)};
	GE_PhysicsRotationToRenderRotation(&position.r);
	SDL_Point axis_sdl = {static_cast<int>(axis.x),static_cast<int>(axis.y)};
	SDL_RenderCopyEx(this->renderer, this->colorTexture, &renderAnimation, &renderPosition,position.r,&axis_sdl,SDL_FLIP_NONE); //RenderCopyEx copies the data from the pointers to the SDL_Rects, meaning they can be discarded immediatly following the function call.
}
void GE_RectangleShape::render(Vector2r position, Vector2 size)
{
	render(position,size,{0,0});
}
void GE_RectangleShape::render(Vector2 position, Vector2 size)
{
	render({position.x,position.y,0},size);
}
void GE_RectangleShape::render(Vector2 start, Vector2 end, double thickness)
{
	Vector2r finalposition;
	Vector2 finalsize;
	//the line is as big as the distance and as thick as thickness
	finalsize.x = GE_Distance(start,end);
	finalsize.y = thickness;
	//the final starting position
	finalposition.x = start.x;
	finalposition.y = start.y-(finalsize.y/2);

	//find the angle between start and end 
	finalposition.r = asin((end.y-start.y)/finalsize.x);
	if (end.x < start.x) //correct for sin 
	{
		finalposition.r += M_PI;
		finalposition.r *= -1;
	}
	//render the line. axis of rotation is at the far left in the center of the thickness. 
	render(finalposition,finalsize,{0,finalsize.y/2});
}

GE_HollowRectangleShape::GE_HollowRectangleShape(SDL_Renderer* renderer, GE_Color color,double thickness)
{
	this->thickness = thickness;
	this->color = new GE_RectangleShape(renderer,color);
}
GE_HollowRectangleShape::~GE_HollowRectangleShape()
{
	delete color;
}
void GE_HollowRectangleShape::render(Vector2r position, Vector2 size)
{
	position.r *= -1; 

	position.x += thickness;
 	position.y += thickness;
 	size.x -=thickness;
 	size.y -=thickness;

	Vector2 points[4];
	GE_RectangleToPoints({0,0,0,size.x,size.y},size,points,position);
	//render the connection betwen all the points in the rectangle
	color->render(points[0]-Vector2{thickness/2,0},points[1]+Vector2{thickness/2,0},thickness);
	color->render(points[1],points[3],thickness);
	color->render(points[2]-Vector2{thickness/2,0},points[3]+Vector2{thickness/2,0},thickness);
	color->render(points[2],points[0],thickness);

}

