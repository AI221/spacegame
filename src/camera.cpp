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

#include "camera.h"


Vector2r GE_ApplyCameraOffset(Camera* camera, Vector2r subject, Vector2 size)
{
	//move their position to be at the camera
	subject.x = (subject.x-camera->pos.x);
	subject.y = (subject.y-camera->pos.y);

	//perform matrix transformation, rotating them arround what-is-currently 0,0
	GE_Vector2Rotation(&subject,camera->pos.r);

	//adjust their position to be at the center of the screen
	subject.x = (subject.x+(camera->screenWidth/2));
	subject.y = (subject.y+(camera->screenHeight/2));

	//their position is rotated, now rotate the sprite itself.
	subject.r = (camera->pos.r)-subject.r; 


	return subject;


}

