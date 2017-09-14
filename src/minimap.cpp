#include "minimap.h"



GE_UI_Minimap::GE_UI_Minimap(SDL_Renderer* renderer, Vector2 position, Vector2 size, SDL_Color background, SDL_Color crosshair) 
{
	this->renderer = renderer;
	this->position = position;
	this->size = size;
	this->background = background;
	this->crosshair = crosshair;


	this->wantsEvents = false;
}
void GE_UI_Minimap::GE_UI_Element::giveEvent(Vector2 parrentPosition, SDL_Event event)
{ // do nothing, we don't take events
}
void GE_UI_Minimap::render(Vector2 parrentPosition)
{

}


