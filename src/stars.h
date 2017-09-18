/*!
 * @file
 * @author Jackson McNeill
 *
 * A system that allows for repeating stars to be created, which aids the player in telling how fast they're moving in an outer space environment
 */

#include <unordered_set>
#include <vector>
#include <math.h>
//Local includes
#include "vector2.h"
#include "shapes.h"
#include "camera.h"




#ifndef __STARS_INCLUDED
#define __STARS_INCLUDED

struct GE_SingleStar
{
	Vector2 position;
	int size;
	unsigned int color; //color from the colorIDs in the starfield

};

struct GE_Stars
{
	SDL_Renderer* renderer;
	std::unordered_set<GE_SingleStar*> stars;
	Vector2 allStarsSize;
	double allStarsScale;
	std::vector<GE_RectangleShape*> starRectangles;
};

GE_Stars* GE_CreateStars(SDL_Renderer* renderer, unsigned int number, unsigned int width, unsigned int height,std::vector<int> sizes, double scale, std::vector<GE_Color> colors);
void GE_FreeStars(GE_Stars* subject);

void GE_BlitStars(GE_Stars* subject, Camera* camera);


#endif // __STARS_INCLUDED
