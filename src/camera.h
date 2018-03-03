/*!
 * @file
 * @author Jackson McNeill
 *
 * A small Camera stucture and a function to apply offset based upon it 
 */
#pragma once

#include <math.h>


#include "vector2.h"




/*!
 * A camera, which contains the position(including rotation) of the camera, and the screen height and width
 */
struct Camera
{
	Camera(Vector2r pos, int screenWidth,int screenHeight)
	{
		this->pos = pos;
		this->screenWidth = screenWidth;
		this->screenHeight = screenHeight;
	}	
	Camera(){}
	Vector2r pos; 
	int screenWidth;
	int screenHeight;
};

/*!
 * Applys camera offset, makes the camera->pos the center of the screen, and applys rotation.
 *
 * The applying of rotation may be unexpected due to many 2D games not doing this; simply set the r in camera->pos to 0 to stop this behaviour.
 */
Vector2r GE_ApplyCameraOffset(Camera* camera, Vector2r subject, Vector2 size);

	


