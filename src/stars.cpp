#include "stars.h"

#define random_range_double(low,high) (  ((rand() % (std::abs(low)+high))+1.0)-low  )
#define wraparround_clamp(number,maxval) (  number-(std::floor(number/maxval)*maxval)  )
#define nwraparround_clamp(number,maxval) (number)

GE_Stars* GE_CreateStars(SDL_Renderer* renderer, unsigned int number, unsigned int width, unsigned int height, Vector2 size, double scale, GE_Color color)
{
	GE_Stars* newStars = new GE_Stars{renderer,std::unordered_set<Vector2*>{},{static_cast<double>(width),static_cast<double>(height)},scale,new GE_RectangleShape(renderer,color),size};
	for (int i =0;i<=number;i++)
	{
		newStars->stars.insert(new Vector2{random_range_double(0,width),random_range_double(0,height)});
	}
	return newStars;
}
void GE_FreeStars(GE_Stars* subject)
{
	for (Vector2* subjectVector: subject->stars)
	{
		delete subjectVector;
	}
	delete subject->starRectangle;
	delete subject;
}

void GE_BlitStars(GE_Stars* subject, Camera* camera)
{
	Camera* scaledcamera;
	Vector2r position;
	position.r = 0; //the rotation is always 0
	for (Vector2* subjectVector: subject->stars)
	{
		scaledcamera = new Camera{Vector2r{camera->pos.x*subject->allStarsScale,camera->pos.y*subject->allStarsScale,camera->pos.r},camera->screenHeight,camera->screenWidth};


		position.x = (subjectVector->x-scaledcamera->pos.x*subject->allStarsScale);
		position.y = (subjectVector->y-scaledcamera->pos.y*subject->allStarsScale);

		position= {wraparround_clamp(position.x,subject->allStarsSize.x),wraparround_clamp(position.y,subject->allStarsSize.y)};
 	
		position.x = position.x-(subject->allStarsSize.x/2);
		position.y = position.y-(subject->allStarsSize.y/2);

		GE_Vector2Rotation(&position,scaledcamera->pos.r);
		delete scaledcamera; //free the memory used by the new scaled camera

		position.x = (position.x+(scaledcamera->screenWidth/2));//-(subject->allStarsSize.x/2);
		position.y = (position.y+(scaledcamera->screenHeight/2));//-(subject->allStarsSize.y/2);

		subject->starRectangle->render(position,subject->starRectangleSize);
	}

}
