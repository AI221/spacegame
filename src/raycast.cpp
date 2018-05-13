#include "raycast.h"

#include "vector2.h"
#include "GeneralEngineCPP.h"




std::optional<Vector2> GE_Raycast(Vector2 start, Vector2 end, GE_ShapeLinesVector obstacles)
{
	std::optional<Vector2> currentShortest = {};
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
				{
					Vector2 find = Vector2{intersectionXVal.value(),intersectionYVal};
					//we have to keep iterating because this might not be the best solution. 
					if(!currentShortest.has_value())
					{
						currentShortest = {find};
					}
					else
					{
						currentShortest = {GE_ClosestVector(start,find,currentShortest.value())};
					}
				}
			}

			lastSpot = spot;
		}
	}
	return currentShortest; //return the last found shortest value, which was initially initialized to NAN,NAN

}


ObjectRaycastReturn GE_Raycast(Vector2 start, Vector2 end, std::vector<GE_PhysicsObject*> objects)
{
	objects = GE_SortPhysicsObjectByClosest_Inaccurate(start,objects);		
	Vector2 currentShortest = end;
	GE_PhysicsObject* currentShortestObj = NULL;
	for (GE_PhysicsObject* obj : objects)
	{
		GE_ShapeLinesVector shapes;
		for (int i=0;i<obj->numCollisionRectangles;i++)
		{
			Vector2 points[4];
			GE_RectangleToPoints(obj->collisionRectangles[i],obj->grid,points,obj->position); //populate points
			GE_ShapeLines shapeLines;
			//connect every point on the rectangle with lines
			shapeLines.insert(shapeLines.end(),points[0]);
			shapeLines.insert(shapeLines.end(),points[1]);
			shapeLines.insert(shapeLines.end(),points[3]);
			shapeLines.insert(shapeLines.end(),points[2]);
			shapeLines.insert(shapeLines.end(),points[0]); 
			shapes.insert(shapes.end(),shapeLines);
		}
		auto result = GE_Raycast(start,end,shapes);
		if (!result.has_value())
		{
			continue;
		}
#ifdef PHYSICS_DEBUG_SLOWRENDERS
		GE_DEBUG_TextAt_PhysicsPosition(std::to_string(GE_Distance(start,result.value())),result.value());
#endif
		if (GE_Distance(start,result.value()) <= GE_Distance(start,currentShortest))
		{
			currentShortest = result.value();
			currentShortestObj = obj;
		}
		else if(currentShortestObj != NULL)
		{
			break;
		}

	}
	return {currentShortestObj,currentShortest};
}


	/*
	std::vector<GE_PhysicsObject*> collisionRectangleToOwner;
	GE_ShapeLinesVector shapes;	
	int ammountShapes = 0;
	for (GE_PhysicsObject* obj : objects)
	{
		for (int i=0;i<obj->numCollisionRectangles;i++)
		{
			Vector2 points[4];
			GE_RectangleToPoints(obj->collisionRectangles[i],obj->grid,points,obj->position);
			GE_ShapeLines shapeLines;
			shapeLines.insert(shapeLines.end(),points[0]);
			shapeLines.insert(shapeLines.end(),points[1]);
			shapeLines.insert(shapeLines.end(),points[3]);
			shapeLines.insert(shapeLines.end(),points[2]);
			shapeLines.insert(shapeLines.end(),points[0]);
			shapes.insert(shapes.end(),shapeLines);
			ammountShapes++;
			collisionRectangleToOwner.insert(collisionRectangleToOwner.end(),obj);
		}

	}
	int shapenumber = GE_Raycast(start,end,shapes).identity;
	return (shapenumber>ammountShapes)? NULL : collisionRectangleToOwner[shapenumber];
	
}

*/
