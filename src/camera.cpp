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
	
	/*double sin_angle = sin((camera->pos.r/DEG_TO_RAD));
	double cos_angle = cos((camera->pos.r/DEG_TO_RAD));

	newPosition.x = ((subject->x-camera->pos.x))*cos_angle - ((subject->y-camera->pos.y))*sin_angle;
	newPosition.y = ((subject->x-camera->pos.x))*sin_angle + ((subject->y-camera->pos.y))*cos_angle;*/


	subject.x = (subject.x-camera->pos.x)+(size.x/2);
	subject.y = (subject.y-camera->pos.y)+(size.y/2);


	GE_Vector2Rotation(&subject,camera->pos.r);

	subject.x = (subject.x+(camera->screenWidth/2))-(size.x/2);
	subject.y = (subject.y+(camera->screenHeight/2))-(size.y/2);

	subject.r = (camera->pos.r-360)-subject.r; //last newPosition.r might not be correct

	subject.r -= floor(subject.r/360)*360;

	return subject;


}
