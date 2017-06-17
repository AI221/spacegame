#include "camera.h"


Vector2r GE_ApplyCameraOffset(Camera* camera, Vector2r* subject)
{
	Vector2r newPosition;
	
	double sin_angle = sin((camera->pos.r/DEG_TO_RAD));
	double cos_angle = cos((camera->pos.r/DEG_TO_RAD));

	newPosition.x = ((subject->x-camera->pos.x))*cos_angle - ((subject->y-camera->pos.y))*sin_angle;
	newPosition.y = ((subject->x-camera->pos.x))*sin_angle + ((subject->y-camera->pos.y))*cos_angle;

	newPosition.x += (camera->screenWidth)/2;
	newPosition.y += (camera->screenHeight)/2;

	newPosition.r = (camera->pos.r-360)-subject->r; //last newPosition.r might not be correct

	newPosition.r -= floor(newPosition.r/360)*360;

	return newPosition;


}
