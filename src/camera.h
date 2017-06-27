#include <math.h>
#include "vector2.h"


#ifndef __CAMERA_INCLUDED
#define __CAMERA_INCLUDED

struct Camera
{
	Vector2r pos; //position
	int screenHeight;
	int screenWidth;
};

Vector2r GE_ApplyCameraOffset(Camera* camera, Vector2r subject);


#endif //__CAMERA_INCLUDED
