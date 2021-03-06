#include "gameRender.h"
#include "physics.h"

#include <pthread.h>

//Local includes
#include "renderedObject.h"


GE_UI_GameRender::GE_UI_GameRender(SDL_Renderer* renderer, Vector2 position, Vector2 size, Camera* camera, double scale)
{
	this->renderer = renderer;
	this->position = position;
	this->size = size;

	this->camera = camera;




	this->wantsEvents = true;

	this->setScale(scale);
	//playerEventStack = &focusObject->threadedEventStack;
}
GE_UI_GameRender::~GE_UI_GameRender()
{

}
void GE_UI_GameRender::giveEvent(Vector2 parrentPosition, SDL_Event event)
{
	//playerEventStack->giveEvent(event);
}
void GE_UI_GameRender::render(Vector2 parrentPosition) 
{
	GE_DeleteRenderedObjectsMarkedForDeletion();
	pthread_mutex_lock(&RenderEngineMutex);

	
	//do not access renderedObject.end() - might cause threading issues if the phyiscs engine adds a object during rendering
	auto last_it = GE_GetLastReadableRenderObjectIterator();
	for (auto object_it = renderedObjects.begin(); object_it != last_it; object_it++)
	{
		GE_BlitRenderedObject(*object_it,camera,scale);
	}
	pthread_mutex_unlock(&RenderEngineMutex);

}
void GE_UI_GameRender::setScale(double scale)
{
	this->scale = scale;
#ifdef PHYSICS_DEBUG_SLOWRENDERS
	this->scale = 1;
#endif
}

bool GE_UI_GameRender::checkIfFocused(int mousex, int mousey)
{
	return checkIfFocused_ForBox(mousex,mousey,position,size);
}
