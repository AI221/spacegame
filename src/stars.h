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
#include "UI.h"




#ifndef __STARS_INCLUDED
#define __STARS_INCLUDED

struct GE_SingleStar
{
	Vector2 position;
	int size;
	unsigned int color; //color from the colorIDs in the starfield

};

class GE_Stars : public GE_UI_Element
{
	public:
		GE_Stars(SDL_Renderer* renderer, unsigned int number, unsigned int width, unsigned int height,std::vector<int> sizes, double scale, std::vector<GE_Color> colors,Camera* camera);
		~GE_Stars();

		void render(Vector2 parrentPosition);

	private:

	SDL_Renderer* renderer;
	std::unordered_set<GE_SingleStar*> stars;
	Vector2 allStarsSize;
	double allStarsScale;
	std::vector<GE_RectangleShape*> starRectangles;
	Camera* camera;
};

#endif // __STARS_INCLUDED
