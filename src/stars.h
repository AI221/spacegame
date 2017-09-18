/*!
 * @file
 * @author Jackson McNeill
 *
 * A system that allows for repeating stars to be created, which aids the player in telling how fast they're moving in an outer space environment
 */

#include <unordered_set>
#include <math.h>
//Local includes
#include "vector2.h"
#include "shapes.h"
#include "camera.h"




#ifndef __STARS_INCLUDED
#define __STARS_INCLUDED
struct GE_Stars
{
	SDL_Renderer* renderer;
	std::unordered_set<Vector2*> stars;
	Vector2 allStarsSize;
	double allStarsScale;
	GE_RectangleShape* starRectangle;
	Vector2 starRectangleSize;
};

GE_Stars* GE_CreateStars(SDL_Renderer* renderer, unsigned int number, unsigned int width, unsigned int height, Vector2 size, double scale, GE_Color color);
void GE_FreeStars(GE_Stars* subject);

void GE_BlitStars(GE_Stars* subject, Camera* camera);


#endif // __STARS_INCLUDED
