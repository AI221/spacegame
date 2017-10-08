#include "stars.h"


GE_Stars* GE_CreateStars(SDL_Renderer* renderer, unsigned int number, unsigned int width, unsigned int height,std::vector<int> sizes, double scale, std::vector<GE_Color> colors)
{
	GE_Stars* newStars = new GE_Stars{renderer,std::unordered_set<GE_SingleStar*>{},{static_cast<double>(width),static_cast<double>(height)},scale,std::vector<GE_RectangleShape*>{}};

	for (auto i=colors.begin();i != colors.end(); i++)
	{
		newStars->starRectangles.insert(newStars->starRectangles.end(),new GE_RectangleShape(renderer, *i)); 
	}
	int numColors = newStars->starRectangles.size();
	printf("numColors %d\n",numColors);
	int numSizes = sizes.size();
	for (int i =0;i<=number;i++)
	{
		newStars->stars.insert(new GE_SingleStar{
				Vector2{random_range_double(0,width),random_range_double(0,height)},
				sizes[  static_cast<int>(random_range_double(0,numSizes-1))  ],
				static_cast<unsigned int>(random_range_double(0,numColors-1)),
				});
	}
	return newStars;
}
void GE_FreeStars(GE_Stars* subject)
{
	for (GE_SingleStar* subjectStar: subject->stars)
	{
		delete subjectStar;
	}
	for (auto i=subject->starRectangles.begin(); i != subject->starRectangles.end(); i++)
	{
		delete *i;
	}
	delete subject;
}

void GE_BlitStars(GE_Stars* subject, Camera* camera)
{
	Camera* scaledcamera;
	Vector2r position;
	position.r = 0; //the rotation is always 0
	for (GE_SingleStar* subjectStar: subject->stars)
	{
		scaledcamera = new Camera{Vector2r{camera->pos.x*subject->allStarsScale,camera->pos.y*subject->allStarsScale,camera->pos.r},camera->screenHeight,camera->screenWidth};


		position.x = (subjectStar->position.x-scaledcamera->pos.x*subject->allStarsScale);
		position.y = (subjectStar->position.y-scaledcamera->pos.y*subject->allStarsScale);

		position= {wraparround_clamp(position.x,subject->allStarsSize.x),wraparround_clamp(position.y,subject->allStarsSize.y)};
 	
		position.x = position.x-(subject->allStarsSize.x/2);
		position.y = position.y-(subject->allStarsSize.y/2);

		GE_Vector2Rotation(&position,scaledcamera->pos.r);
		delete scaledcamera; //free the memory used by the new scaled camera

		position.x = (position.x+(scaledcamera->screenWidth/2));//-(subject->allStarsSize.x/2);
		position.y = (position.y+(scaledcamera->screenHeight/2));//-(subject->allStarsSize.y/2);

		if ( ( position.x+subjectStar->size >= 0) && (position.x-subjectStar->size <= scaledcamera->screenWidth) && (position.y+subjectStar->size >= 0) && (position.y-subjectStar->size <= scaledcamera->screenHeight))
		{
			subject->starRectangles[subjectStar->color]->render(position,{static_cast<double>(subjectStar->size),static_cast<double>(subjectStar->size)});
		}
	}

}
