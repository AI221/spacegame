/*
Copyright 2017 Jackson Reed McNeill

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "renderedObject.h"

#include <stack>

#include "sprite.h"
#include "camera.h"
#include "gluePhysicsObject.h"


#include "GeneralEngineCPP.h"



#ifdef GE_DEBUG
//#define debug_renderedobject
#endif
pthread_mutex_t RenderEngineMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t deleteObjectStackMutex = PTHREAD_MUTEX_INITIALIZER;

rendered_objects_list_t renderedObjects;


/*int numRenderedObjects = -1; //Writable by the physics engine AND render engine -- must lock createObjectMutex to write
int numRenderedObjectsReadable = -1; //pulled on render tick; render objects gaurnteed not to be messed with by the physics engine. DO NOT WRITE TO! Let this be managed by glue.
//Reason: A race condition could occur where the physics engine starts creating a render object, then the renderer does immediatly after. Physics engine finished and unlocks createObjectMutex, then render engine goes through and finishes and sets numRenderedObjectsReadable to numRenderedObjects. But Physics Engine was still touching its renderedObject, and hadn't finished the tick, thus a race condition has occured. Render engine doesn't need this protection cause it is only 1 thread, and cannot be touching a rendered object and rendering things simultaniously.*/
rendered_objects_list_t::iterator lastObject;
rendered_objects_list_t::iterator lastReadableObject;

GE_GlueTarget* numRenderedObjectsToNumRenderedObjectReadableGlue;

pthread_mutex_t createObjectMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t listShiftMutex = PTHREAD_MUTEX_INITIALIZER;

std::stack<GE_RenderedObject*> scheduleToDelete;





int GE_RenderedObjectInit()
{
	numRenderedObjectsToNumRenderedObjectReadableGlue = GE_addGlueSubject(&lastReadableObject,&lastObject,GE_PULL_ON_RENDER,sizeof(lastReadableObject));

	return 0;
}


void GE_ChangeRenderedObjectSprite(GE_RenderedObject* subject, std::string spriteName)
{
	subject->spriteID = GE_SpriteNameToID(spriteName);	
}


GE_RenderedObject* GE_ScheduleCreateRenderedObject(SDL_Renderer* renderer, std::string spriteName)
{
	pthread_mutex_lock(&createObjectMutex);
	GE_RenderedObject* returnval = GE_CreateRenderedObject(renderer,spriteName); //do everything the same -- numRenderedObjectsReadable will not be updated till the physics thread finishes this tick
	pthread_mutex_unlock(&createObjectMutex);
	
	return returnval;
}

GE_RenderedObject* GE_CreateRenderedObject(SDL_Renderer* renderer, std::string spriteName) // size is not included (despite it being a value often set at start) due to its linked nature.
{
	printf("Create RenderedObject\n");
	GE_RenderedObject* renderedObject = new GE_RenderedObject{renderer, -1, Vector2r{0,0,0}, Vector2{0,0},GE_Rectangle{0,0,0,0}};
	GE_ChangeRenderedObjectSprite(renderedObject,spriteName);
	pthread_mutex_lock(&listShiftMutex);
	renderedObjects.insert(renderedObjects.end(), renderedObject);
	lastObject = renderedObjects.end();
	pthread_mutex_unlock(&listShiftMutex);
	return renderedObject;
}
void GE_BlitRenderedObject(GE_RenderedObject* subject, Camera* camera, double scale)
{
	Camera* scaledcamera = new Camera{Vector2r{camera->pos.x*scale,camera->pos.y*scale,camera->pos.r},camera->screenHeight,camera->screenWidth};
	Vector2r position = GE_ApplyCameraOffset(scaledcamera,{subject->position.x*scale,subject->position.y*scale,subject->position.r},{subject->size.x*scale, subject->size.y*scale});
	delete scaledcamera;
	double maxSize = std::max(subject->size.x,subject->size.y);
	if ( ( position.x+maxSize >= 0) && (position.x-maxSize <= scaledcamera->screenWidth) && (position.y+maxSize >= 0) && (position.y-maxSize <= scaledcamera->screenHeight))
	{
		GE_BlitSprite(Sprites[subject->spriteID],position,{subject->size.x*scale, subject->size.y*scale},subject->animation,GE_FLIP_NONE); //TODO
	}
#ifdef debug_renderedobject
	else GE_DEBUG_TextAt("norender!~~~~~~~~~~~~~~~~~~~~~~~~`~~~~~norender!",Vector2{position.x-100,position.y-100});
	
	GE_DEBUG_TextAt(std::to_string(position.x) + "," + std::to_string(position.y) + ",m "+std::to_string(maxSize),Vector2{position.x,position.y});
#endif
}
void GE_FreeRenderedObject(GE_RenderedObject* subject) //will not destroy renderer,or sprite. MUST be allocated with new
{
	pthread_mutex_lock(&listShiftMutex);
	renderedObjects.remove(subject);
	lastObject = renderedObjects.end();
	lastReadableObject = lastObject;
	pthread_mutex_unlock(&listShiftMutex);

	delete subject;
}
void GE_ScheduleFreeRenderedObject(GE_RenderedObject* subject)
{
	printf("scheduling %d\n",subject);
	pthread_mutex_lock(&deleteObjectStackMutex);

	scheduleToDelete.push(subject);

	pthread_mutex_unlock(&deleteObjectStackMutex);

}
void GE_DeleteRenderedObjectsMarkedForDeletion()
{
	printf("entr\n");

	pthread_mutex_lock(&deleteObjectStackMutex);
	while (!scheduleToDelete.empty())
	{
		GE_RenderedObject* i = scheduleToDelete.top();
		scheduleToDelete.pop();

		printf("freeing %d\n",i);
		GE_FreeRenderedObject(i);
	}
	pthread_mutex_unlock(&deleteObjectStackMutex);
	printf("exit\n");
}


void GE_ShutdownRenderedObject()
{
	GE_FreeGlueObject(numRenderedObjectsToNumRenderedObjectReadableGlue);
}
