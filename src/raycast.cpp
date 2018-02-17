#include "raycast.h"

#include "vector2.h"
#include "GeneralEngineCPP.h"

Vector2 GE_Raycast(Vector2 start, Vector2 end, GE_ShapeLinesVector obstacles)
{
	Vector2 currentShortest = {NAN,NAN};
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
			double intersectionXVal = GE_LineIntersection(rayLine.m,rayLine.b,shapeLine.m,shapeLine.b,((static_cast<unsigned char>(rayLine.mIsInf))+(static_cast<unsigned char>(shapeLine.mIsInf)*2)));
			//check if it's real, and in the x range of the ray and shape line
			if (!std::isnan(intersectionXVal) && GE_IsInRange(intersectionXVal,start.x,end.x) && GE_IsInRange(intersectionXVal,lastSpot->x,spot->x))
			{
				//check in y range of shape line -- do not use a vertical line
				double intersectionYVal = shapeLine.mIsInf? (rayLine.m*intersectionXVal)+rayLine.b : (shapeLine.m*intersectionXVal)+shapeLine.b;
				if (GE_IsInRange(intersectionYVal,start.y,end.y) && GE_IsInRange(intersectionYVal,lastSpot->y,spot->y))
				{
					Vector2 find = Vector2{intersectionXVal,intersectionYVal};
					//we have to keep iterating because this might not be the best solution. 
					currentShortest = (std::isnan(currentShortest.x) || std::isnan(currentShortest.y))? find :  GE_ClosestVector(start,find,currentShortest);
				}
			}

			lastSpot = spot;
		}
	}
	return currentShortest; //return the last found shortest value, which was initially initialized to NAN,NAN

}


