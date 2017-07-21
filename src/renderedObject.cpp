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

RenderedObject* renderedObjects[1000]; //TODO dimensions and what not
int countRenderedObjects = -1; 

RenderedObject* GE_CreateRenderedObject(SDL_Renderer* renderer, std::string spriteName) // size is not included (despite it being a value often set at start) due to its linked nature.
{
	countRenderedObjects++;
	RenderedObject* renderedObject = new RenderedObject{renderer, GE_SpriteNameToID(spriteName), Vector2r{0,0,0}, Vector2{0,0}, Animation{0,0,0,0}, LINKED_NONE }; //TODO: some error handling for sprite 
	renderedObjects[countRenderedObjects] = renderedObject;
	return renderedObject;
}
void GE_BlitRenderedObject(RenderedObject* subject, Camera* camera)
{
	Vector2r position = GE_ApplyCameraOffset(camera,subject->position);
	GE_BlitSprite(Sprites[subject->spriteID],position,subject->size,subject->animation,FLIP_NONE); //TODO

}
void GE_FreeRenderedObject(RenderedObject* subject) //will not destroy renderer,or sprite. MUST be allocated with new
{
	delete subject;
}


