#include "gameRender.h"

GE_UI_GameRender::GE_UI_GameRender(SDL_Renderer* renderer, Vector2 position, Vector2 size, Player* focusObject)
{
	this->renderer = renderer;
	this->position = position;
	this->size = size;


	this->camera.pos = {0,0,0};

	this->camera.screenHeight = size.x;
	this->camera.screenWidth = size.y;

	this->camerasGrid = {0,0,0,0};


	this->wantsEvents = true;


	GE_LinkVectorToPhysicsObjectPosition(focusObject,&(camera.pos));
	GE_LinkGlueToPhysicsObject(focusObject,GE_addGlueSubject(&camerasGrid,&focusObject->grid,GE_PULL_ON_PHYSICS_TICK,sizeof(GE_Rectangle)) );
	playerEventStack = &focusObject->threadedEventStack;
}
GE_UI_GameRender::~GE_UI_GameRender()
{

}
void GE_UI_GameRender::giveEvent(Vector2 parrentPosition, SDL_Event event)
{
	playerEventStack->giveEvent(event);
}
void GE_UI_GameRender::render(Vector2 parrentPosition)
{
	pthread_mutex_lock(&RenderEngineMutex);

	camera.pos.x = (camera.pos.x+camerasGrid.w/2); //manipulate camera position now that it's updated
	camera.pos.y = (camera.pos.y+camerasGrid.h/2);
	#ifdef NO_CAMERA_ROTATE
		camera.pos.r = 0;
	#endif
	for (int i=0; i <= numRenderedObjects; i++)
	{
		if (!deadRenderedObjects[i])
		{
			GE_BlitRenderedObject(renderedObjects[i],&camera,0.75);
		}
	}
	pthread_mutex_unlock(&RenderEngineMutex);

}

bool GE_UI_GameRender::checkIfFocused(int mousex, int mousey)
{
	if (mousex >= position.x && mousex <= position.x+size.x && mousey >= position.y && mousey <= position.y+size.y)
	{
		return true;
	}
	return false;
}
