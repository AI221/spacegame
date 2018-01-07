#include "minimap.h"

struct GE_MinimapTarget //exists to allow for future expansion
{
	SDL_Color color;
};

std::unordered_map<GE_RenderedObject*,GE_MinimapTarget*> minimapTargets; //this works PERFECTLY for what I need it for--it requires no additional variables for the user to store, just tell us what linked rendered object you're deleting before you delete it... genius.

std::stack<GE_RenderedObject*> scheduledToDelete;

pthread_mutex_t deleteStackMutex = PTHREAD_MUTEX_INITIALIZER;



void GE_LinkMinimapToRenderedObject(GE_RenderedObject* subject, SDL_Color color)// TODO threadsafe sheduling for physics engine
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

	//remove anything scheduled for removal
	
	deleteTargetsScheduledForRemoval();

	for (auto i : minimapTargets)
	{
		//camera gets scaled way down to fit a minimap
		Camera scaledcamera = Camera{Vector2r{camera->pos.x*scale,camera->pos.y*scale,camera->pos.r},static_cast<int>(size.y),static_cast<int>(size.x)};

		printf("RenderATarget\n");
		GE_MinimapTarget* subject_minimap = i.second;
		if (subject_minimap != NULL)
		{
			GE_RenderedObject* subject = i.first;
			SDL_Color color = subject_minimap->color;

			//apply camera offset
			Vector2r position = GE_ApplyCameraOffset(&scaledcamera,{subject->position.x*scale,subject->position.y*scale,subject->position.r},{subject->size.x*scale, subject->size.y*scale});
			if ((position.x>=effectivePosition.x)&&(position.y>=effectivePosition.y)&&(position.x<=size.x)&&(position.y<=size.y)) //check if the object is within the bounds of the minimap
			{
				//render the object
				SDL_SetRenderDrawColor(renderer,color.r, color.g, color.b, color.a);
				temprect = SDL_Rect{static_cast<int>(position.x),static_cast<int>(position.y),3,3};
				SDL_RenderFillRect(renderer,&temprect);
			}	
		}
		else { printf("WTF NULL!!!\n");}
	}
}


