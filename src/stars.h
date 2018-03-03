/*!
 * @file
 * @author Jackson McNeill
 *
 * A system that allows for repeating stars to be created, which aids the player in telling how fast they're moving in an outer space environment
 */
#pragma once

#include "vector2.h"
#include "UI.h"
#include <unordered_set>
#include <vector>

class Camera;

class SDL_Renderer;




struct GE_SingleStar;

class GE_Stars : public GE_UI_Element
{
	public:
		GE_Stars(SDL_Renderer* renderer, unsigned int number, unsigned int width, unsigned int height,std::vector<int> sizes, double scale, std::vector<GE_Color> colors,Camera* camera);
		~GE_Stars();

		void render(Vector2 parrentPosition);
		void setScale(double scale);

	private:

		SDL_Renderer* renderer;
		std::unordered_set<GE_SingleStar*> stars;
		Vector2 allStarsSize;
		double allStarsScale;
		std::vector<GE_RectangleShape*> starRectangles;
		Camera* camera;
};


