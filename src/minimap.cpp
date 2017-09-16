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
	Vector2 effectivePosition = parrentPosition+position;
	//render background rect
	SDL_SetRenderDrawColor(renderer,background.r, background.g, background.b, background.a);
	SDL_Rect temprect = SDL_Rect{static_cast<int>(effectivePosition.x),static_cast<int>(effectivePosition.y),static_cast<int>(size.x),static_cast<int>(size.y)};
	SDL_RenderFillRect(renderer,&temprect);
	//render crosshair
	SDL_SetRenderDrawColor(renderer,crosshair.r, crosshair.g, crosshair.b, crosshair.a);
	//Y crosshair
	temprect = SDL_Rect{static_cast<int>(effectivePosition.x+(size.x/2)),static_cast<int>(effectivePosition.y),1,static_cast<int>(size.y)};
	SDL_RenderFillRect(renderer,&temprect);
	//X crosshair
	temprect = SDL_Rect{static_cast<int>(effectivePosition.x),static_cast<int>(effectivePosition.y+(size.y/2)),static_cast<int>(size.x),1};
	SDL_RenderFillRect(renderer,&temprect);


	for (int i=0; i <= numRenderedObjects; i++)
	{
		GE_RenderedObject* subject = renderedObjects[i];
		if (!deadRenderedObjects[i])
		{
			Camera* scaledcamera = new Camera{Vector2r{camera->pos.x*scale,camera->pos.y*scale,camera->pos.r},static_cast<int>(size.y),static_cast<int>(size.x)};
			Vector2r position = GE_ApplyCameraOffset(scaledcamera,{subject->position.x*scale,subject->position.y*scale,subject->position.r},{subject->size.x*scale, subject->size.y*scale});
			delete scaledcamera;
			if ((position.x>=effectivePosition.x)&&(position.y>=effectivePosition.y)&&(position.x<=size.x)&&(position.y<=size.y))
			{
				SDL_SetRenderDrawColor(renderer,0x66,0xFF,0x00,0xFF);//background.r, background.g, background.b, background.a);
				temprect = SDL_Rect{static_cast<int>(position.x),static_cast<int>(position.y),2,2};
				SDL_RenderFillRect(renderer,&temprect);
			}


		}	

	}

}


