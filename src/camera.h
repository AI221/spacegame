/*!
 * @file
 * @author Jackson McNeill
 *
 * A small Camera stucture and a function to apply offset based upon it 
 */
#include <math.h>


#include "vector2.h"


#ifndef __CAMERA_INCLUDED
#define __CAMERA_INCLUDED


/*!
 * A camera, which contains the position(including rotation) of the camera, and the screen height and width
 */
struct Camera
{
	Vector2r pos; 
	int screenHeight;
	int screenWidth;
};

/*!
 * Applys camera offset, makes the camera->pos the center of the screen, and applys rotation.
 *
 * The applying of rotation may be unexpected due to many 2D games not doing this; simply set the r in camera->pos to 0 to stop this behaviour.
 */
Vector2r GE_ApplyCameraOffset(Camera* camera, Vector2r subject);


#endif //__CAMERA_INCLUDED
