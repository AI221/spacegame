#include "minimap.h"

#include <SDL2/SDL.h>
#include <pthread.h>
#include <unordered_map>
#include <stack>
#include "camera.h"
#include "dev-exception.h"

struct GE_MinimapTarget //exists to allow for future expansion
{
	GE_Color color;
};

std::unordered_map<GE_RenderedObject*,GE_MinimapTarget*> minimapTargets; //this works PERFECTLY for what I need it for--it requires no additional variables for the user to store, just tell us what linked rendered object you're deleting before you delete it

std::stack<GE_RenderedObject*> scheduledToDelete;

pthread_mutex_t deleteStackMutex = PTHREAD_MUTEX_INITIALIZER;



void GE_LinkMinimapToRenderedObject(GE_RenderedObject* subject, GE_Color color)// TODO threadsafe sheduling for physics engine
{
	GE_MinimapTarget* newTarget = new GE_MinimapTarget{color};
	minimapTargets[subject] = newTarget;
}
void GE_FreeMinimapTarget(GE_RenderedObject* linkedRenderedObject)
{
	delete minimapTargets[linkedRenderedObject];
	minimapTargets.erase(linkedRenderedObject);
}

void GE_ScheduleFreeMinimapTarget(GE_RenderedObject* linkedRenderedObject)
{
	pthread_mutex_lock(&deleteStackMutex);

	scheduledToDelete.push(linkedRenderedObject);	

	pthread_mutex_unlock(&deleteStackMutex);

}
//internal function - not exposed
void deleteTargetsScheduledForRemoval()
{
	pthread_mutex_lock(&deleteStackMutex);
	while (!scheduledToDelete.empty())
	{
		GE_RenderedObject* i = scheduledToDelete.top();
		scheduledToDelete.pop();

		GE_FreeMinimapTarget(i);
	}
	pthread_mutex_unlock(&deleteStackMutex);
}
GE_UI_Minimap::GE_UI_Minimap(SDL_Renderer* renderer, Vector2 position, Vector2 size, double scale, GE_Color background, GE_Color crosshair, Camera* camera) 
	: 
	background(renderer,position,size,background), 
	crosshair_x(renderer,Vector2{0,size.y/2}+position,{size.x,1},crosshair), 
	crosshair_y(renderer,Vector2{size.y/2,0}+position,{1,size.y},crosshair)
{
	this->renderer = renderer;
	this->position = position;
	this->size = size;
	this->scale = scale;
	this->camera = camera;

	this->wantsEvents = false;
}
void GE_UI_Minimap::render(Vector2 parrentPosition)
{

	background.render(parrentPosition);
	crosshair_x.render(parrentPosition);
	crosshair_y.render(parrentPosition);




	//remove anything scheduled for removal
	
	deleteTargetsScheduledForRemoval(); //TODO: This will cause games that do not use the minimap to have tons of removal shedules to just pile up

	SDL_Rect temprect;
	Vector2 effectivePosition = parrentPosition+position;
	for (auto i : minimapTargets)
	{
		//camera gets scaled way down to fit a minimap
		Camera scaledcamera = Camera{Vector2r{camera->pos.x*scale,camera->pos.y*scale,camera->pos.r},static_cast<int>(size.y),static_cast<int>(size.x)};

		GE_MinimapTarget* subject_minimap = i.second;
		if (subject_minimap != NULL)
		{
			GE_RenderedObject* subject = i.first;
			GE_Color color = subject_minimap->color;

			//apply camera offset
			Vector2r position = GE_ApplyCameraOffset(&scaledcamera,{subject->position.x*scale,subject->position.y*scale,subject->position.r});
			if ((position.x>=effectivePosition.x)&&(position.y>=effectivePosition.y)&&(position.x<=size.x)&&(position.y<=size.y)) //check if the object is within the bounds of the minimap
			{
				//render the object
				SDL_SetRenderDrawColor(renderer,color.r, color.g, color.b, color.a);
				temprect = SDL_Rect{static_cast<int>(position.x),static_cast<int>(position.y),3,3};
				SDL_RenderFillRect(renderer,&temprect);
			}	
		}
		else 
		{ 
			throw new GE_DevException("Minimap target was null - most likely a minimap object that was registered got deleted and failed to remove itself from the minimap");
		}
	}
}
