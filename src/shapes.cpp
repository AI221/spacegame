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
	Vector2 pointSizes[4];
	points[0] = {0,0}; //middle top
	pointSizes[0] = {size.x,thickness};
	points[1] = {size.x,0}; //middle right
	pointSizes[1] = {thickness,size.y};
	points[2] = {0,size.y}; //middle bottom
	pointSizes[2] = {size.x,thickness};
	points[3] = {0,size.y}; //middle left
	pointSizes[1] = {thickness,size.y};

	for(int i=0;i!=4;i++)
	{
		points[i] = points[i]+position;
		points[i].x -= size.x/2;
		points[i].y -= size.y/2;
		points[i] = points[i]-(pointSizes[i]/2);
		GE_Vector2RotationCCW(&points[i],position.r);
		//points[i] = points[i]+(pointSizes[i]/2);
		points[i].x += size.x/2;
		points[i].y += size.y/2;
	}

	GE_PhysicsRotationToRenderRotation(&position.r);
	for(int i=0;i!=4;i++)
	{
		renderPosition = {static_cast<int>(points[i].x+.5),static_cast<int>(points[i].y+.5),static_cast<int>(pointSizes[i].x),static_cast<int>(pointSizes[i].y)};
		SDL_RenderCopyEx(this->renderer, this->colorTexture, &renderAnimation, &renderPosition,(position.r),&center,SDL_FLIP_NONE); 
	}
}
