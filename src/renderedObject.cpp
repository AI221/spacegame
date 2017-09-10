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

pthread_mutex_t RenderEngineMutex = PTHREAD_MUTEX_INITIALIZER;

GE_RenderedObject* renderedObjects[MAX_RENDER_OBJECTS]; //TODO dimensions and what not
bool deadRenderedObjects[MAX_RENDER_OBJECTS] = {1,};
int numRenderedObjects = -1; 


int GE_RenderedObjectInit()
{
	memset(deadRenderedObjects,1,sizeof(deadRenderedObjects)); //Fill deadRendredObjects with 1, so that any unintialized object is "dead"
	return 0;
}

GE_RenderedObject* GE_CreateRenderedObject(SDL_Renderer* renderer, std::string spriteName, int ID) // size is not included (despite it being a value often set at start) due to its linked nature.
{
	printf("Create RenderedObject\n");
	GE_RenderedObject* renderedObject = new GE_RenderedObject{renderer, GE_SpriteNameToID(spriteName), Vector2r{0,0,0}, Vector2{0,0}, GE_Rectangle{0,0,0,0}, };
	renderedObjects[ID] = renderedObject;
	pthread_mutex_lock(&RenderEngineMutex);
	deadRenderedObjects[ID] = false; //Mark us as alive
	numRenderedObjects = std::max(numRenderedObjects,ID); //Increase the num rendered objects to at least our ID
	pthread_mutex_unlock(&RenderEngineMutex);
	return renderedObject;
}
GE_RenderedObject* GE_CreateRenderedObject(SDL_Renderer* renderer, std::string spriteName) 
{
	return GE_CreateRenderedObject(renderer,spriteName,numRenderedObjects+1);
}
void GE_BlitRenderedObject(GE_RenderedObject* subject, Camera* camera)
{
	Vector2r position = GE_ApplyCameraOffset(camera,subject->position,subject->size);
	GE_BlitSprite(Sprites[subject->spriteID],position,subject->size,subject->animation,GE_FLIP_NONE); //TODO

}
void GE_FreeRenderedObject(GE_RenderedObject* subject) //will not destroy renderer,or sprite. MUST be allocated with new
{
	delete subject;
}


