#include "gameRender.h"
#ifdef PHYSICS_DEBUG_SLOWRENDERS
#define NO_myCamera_ROTATE
#endif

GE_UI_GameRender::GE_UI_GameRender(SDL_Renderer* renderer, Vector2 position, Vector2 size, Camera* camera)
{
	this->renderer = renderer;
	this->position = position;
	this->size = size;

	this->camera = camera;




	this->wantsEvents = true;


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

	#ifdef NO_myCamera_ROTATE
		myCamera.pos.r = 0;
	#endif
	for (GE_RenderedObject* object : renderedObjects)
	{
		GE_BlitRenderedObject(object,camera,0.75);
	}
	pthread_mutex_unlock(&RenderEngineMutex);

}

bool GE_UI_GameRender::checkIfFocused(int mousex, int mousey)
{
	return checkIfFocused_ForBox(mousex,mousey,position,size);
}
