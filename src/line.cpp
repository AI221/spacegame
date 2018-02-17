#include "line.h"


/*!
 * Gets the slope-intecept form of a line. Guranteed to return 0 for m and start.x for b if mIsInf is true
 */
GE_Line GE_GetLine(Vector2 start, Vector2 end)
{
	if (start.x == end.x)
	{
		return GE_Line{0,start.x,true};//line{start.y,0,true};
	}
	double slope = (end.y-start.y)/(end.x-start.x);
	double b = start.y-(slope*start.x);
	return GE_Line{slope,b,false};
}


/*!
 * m1,b1,m2,b2 are values for each side of the y=mx+b line
 * infValue is either 0,1,2, or 3 for whether m1 or m2 is equal to infinity. 0 for neither, 1 or 2 for their respectives, 3 for both. Anything higher is undefined behaviour. If no intersection exists, NAN is returned
 */
double GE_LineIntersection(double m1,double b1,double m2,double b2,unsigned char infValue)
{
	switch(infValue)
	{
		case(0):
			return (b1-b2)/(m2-m1);
			break;
		case(1):
			return b1;
			break;
		case(2):
			return b2;
			break;
		case(3):
			if (m1==m2)
			{
				return b1;
			}
			return NAN;
			break;
	}
	return NAN;
}

