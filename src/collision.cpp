#include "collision.h"

/*
bool doShapesCollide(GE_ShapeLinesVector shapes)
{
	for (auto shape : obstacles)
	{
		if (shape.begin() == shape.end())
		{
			//printf("error - shape end equals begin\n");
			break;
		}
		GE_Line rayLine = GE_GetLine(start,end);
		auto lastSpot = shape.begin();
		for (auto spot = shape.begin()+1;spot!= shape.end();spot++)
		{
			GE_Line shapeLine = GE_GetLine(*lastSpot,*spot);	

			//check intersection. if a line(or both) is infinite, tell intersection about this. 
			std::optional<double> intersectionXVal = GE_LineIntersection(rayLine.m,rayLine.b,shapeLine.m,shapeLine.b,((static_cast<unsigned char>(rayLine.mIsInf))+(static_cast<unsigned char>(shapeLine.mIsInf)*2)));
			//check if it's real, and in the x range of the ray and shape line
			if (intersectionXVal.has_value() && GE_IsInRange(intersectionXVal.value(),start.x,end.x) && GE_IsInRange(intersectionXVal.value(),lastSpot->x,spot->x))
			{
				//check in y range of shape line -- do not use a vertical line
				double intersectionYVal = shapeLine.mIsInf? (rayLine.m*intersectionXVal.value())+rayLine.b : (shapeLine.m*intersectionXVal.value())+shapeLine.b;
				if (GE_IsInRange(intersectionYVal,start.y,end.y) && GE_IsInRange(intersectionYVal,lastSpot->y,spot->y))


			}
		}
	}
}

*/
