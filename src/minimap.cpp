#include "minimap.h"



GE_UI_Minimap::GE_UI_Minimap(SDL_Renderer* renderer, Vector2 position, Vector2 size, double scale, SDL_Color background, SDL_Color crosshair, Camera* camera) 
{
	this->renderer = renderer;
	this->position = position;
	this->size = size;
	this->scale = scale;
	this->background = background;
	this->crosshair = crosshair;
	this->camera = camera;



	this->wantsEvents = false;
}
void GE_UI_Minimap::giveEvent(Vector2 parrentPosition, SDL_Event event)
{ // do nothing, we don't take events
}
void GE_UI_Minimap::render(Vector2 parrentPosition)
{
	for (int i=0; i <= numRenderedObjects; i++)
	{
		if (!deadRenderedObjects[i])
		{
			printf("minimaprender %f\n",scale);
			Vector2r position = GE_ApplyCameraOffset(camera,renderedObjects[i]->position,renderedObjects[i]->size);
			GE_BlitSprite(Sprites[renderedObjects[i]->spriteID],{position.x*scale,position.y*scale, position.r},{renderedObjects[i]->size.x*scale,renderedObjects[i]->size.y*scale},{renderedObjects[i]->animation.x/1,renderedObjects[i]->animation.y/1,renderedObjects[i]->animation.w/1,renderedObjects[i]->animation.h/1},GE_FLIP_NONE);
		}	

	}

}


