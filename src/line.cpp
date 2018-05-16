#include "line.h"


/*!
 * Gets the slope-intecept form of a line. Guranteed to return 0 for m and start.x for b if vertical is true
 */
constexpr GE_Line GE_GetLine(Vector2 start, Vector2 end)
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
constexpr std::optional<double> GE_LineIntersection(double m1,double b1,double m2,double b2,unsigned char infValue)
{
	switch(infValue)
	{
		case(0):
			return {(b1-b2)/(m2-m1)};
			break;
		case(1):
			return {b1};
			break;
		case(2):
			return {b2};
			break;
		case(3):
			if (m1==m2)
			{
				return {b1};
			}
			return {};
			break;
	}
	return {};
}

constexpr GE_FORCE_INLINE double calcYVal(double x, GE_Line NonInfLine)
{
	return (NonInfLine.m*x)+NonInfLine.b;
}

constexpr std::optional<Vector2> GE_LineIntersectionVector(GE_Line line_one, GE_Line line_two)
{
	if (line_one.vertical && line_two.vertical)
	{
		return {};	
	}
	else if (line_two.vertical)
	{
		Vector2 returnval;
		returnval.x = line_two.b;
		returnval.y = calcYVal(returnval.x,line_one);
		return {{line_two.b,calcYVal(line_two.b,line_one)}};
	}
	else if(line_one.vertical)
	{
		Vector2 returnval;
		returnval.x = line_one.b; //the line is x=b, so the collision should accur at b.
		returnval.y = calcYVal(returnval.x,line_two);
		return {returnval};
	}
	else
	{
		Vector2 returnval;
		returnval.x =(line_one.b-line_two.b)/(line_two.m-line_one.m);
		returnval.y = calcYVal(returnval.x,line_two);
		return {returnval};
	}
	/*
	switch(static_cast<unsigned char>(line_one.vertical)+(2*static_cast<unsigned char>(line_two.vertical)))
	{
		case(0):
			returnval = {(line_one.b-line_two.b)/(line_one.m-line_two.m),0};
			returnval.y = calcYVal(returnval.x,line_one);
			break;
		case(1):
			returnval = {line_one.b,0};
			returnval.y = calcYVal(returnval.x,line_two);
			break;
		case(2):
			returnval = {line_two.b,0};
			returnval.y = calcYVal(returnval.x,line_one);
			break;
		case(3): //TODO handle for when m1==m2
			return {};
			break;
	}
	return {returnval};
	*/
}
