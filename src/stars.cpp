#include "stars.h"

#include <math.h>
//Local includes
#include "shapes.h"
#include "camera.h"


struct GE_SingleStar
{
	Vector2 position;
	int size;
	unsigned int color; //color from the colorIDs in the starfield

};


//TODO: ideas-
/*Apply the same "repeat star field" thing but to a whole pre-rendered plate of stars
 * Procedurally generate stars as you fly along
 */



GE_Stars::GE_Stars(SDL_Renderer* renderer, unsigned int number, unsigned int width, unsigned int height,std::vector<int> sizes, double scale, std::vector<GE_Color> colors,Camera* camera)
{
	this->renderer = renderer;

	this->stars = std::unordered_set<GE_SingleStar*>{};

	this->allStarsSize = {static_cast<double>(width),static_cast<double>(height)};

	this->allStarsScale = scale;

	this->starRectangles = std::vector<GE_RectangleShape*>{};

	this->camera = camera;

	for (auto i=colors.begin();i != colors.end(); i++)
	{
		starRectangles.insert(starRectangles.end(),new GE_RectangleShape(renderer, *i)); 
	}
	int numColors = starRectangles.size();
	int numSizes = sizes.size();
	for (int i =0;i<=number;i++)
	{
		stars.insert(new GE_SingleStar{
				Vector2{random_range_double(0,width),random_range_double(0,height)},
				sizes[  static_cast<int>(random_range_double(0,numSizes-1))  ],
				static_cast<unsigned int>(random_range_double(0,numColors-1)),
				});
	}
}
GE_Stars::~GE_Stars()
{
	for (GE_SingleStar* subjectStar : stars)
	{
		delete subjectStar;
	}
	for (auto i=starRectangles.begin(); i != starRectangles.end(); i++)
	{
		delete *i;
	}
}

void GE_Stars::render(Vector2 parrentPosition) // TODO:parrentPosition
{
	Camera scaledcamera = Camera{Vector2r{camera->pos.x*allStarsScale,camera->pos.y*allStarsScale,camera->pos.r},camera->screenWidth,camera->screenHeight};
	Vector2r position;
	position.r = 0; //the rotation is always 0
	for (GE_SingleStar* subjectStar: stars)
	{
		position.x = (subjectStar->position.x-scaledcamera.pos.x*allStarsScale);
		position.y = (subjectStar->position.y-scaledcamera.pos.y*allStarsScale);

		position = Vector2r{wraparround_clamp(position.x,allStarsSize.x),wraparround_clamp(position.y,allStarsSize.y),0};
 	
		position.x = position.x-(allStarsSize.x/2);
		position.y = position.y-(allStarsSize.y/2);

		GE_Vector2Rotation(&position,scaledcamera.pos.r);

		position.x = (position.x+(scaledcamera.screenWidth/2));//-(allStarsSize.x/2);
		position.y = (position.y+(scaledcamera.screenHeight/2));//-(allStarsSize.y/2);

		if ( ( position.x+subjectStar->size >= 0) && (position.x-subjectStar->size <= scaledcamera.screenWidth) && (position.y+subjectStar->size >= 0) && (position.y-subjectStar->size <= scaledcamera.screenHeight))
		{
			starRectangles[subjectStar->color]->render(position,{static_cast<double>(subjectStar->size),static_cast<double>(subjectStar->size)});
		}
	}

}
void GE_Stars::setScale(double scale)
{
	allStarsScale = scale;
}
