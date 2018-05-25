#include "raycast.h"

#include "vector2.h"
#include "GeneralEngineCPP.h"

#include "debugRenders.h"





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
		for (auto spot = std::begin(shape);spot!= std::end(shape);spot++)
		{
			GE_Line shapeLine = GE_GetLine(*lastSpot,*spot);	
			//SDL_RenderPresent(GE_DEBUG_Renderer);
			//GE_DEBUG_DrawLine(rayLine);
			std::optional<Vector2> runnerUp = GE_LineIntersectionVector(rayLine,shapeLine);

			//ensure collision is in range of ray, and shape
			if (runnerUp.has_value() && GE_IsInRange(runnerUp.value().x,start.x,end.x) && GE_IsInRange(runnerUp.value().x,lastSpot->x,spot->x))
			{
				if (GE_IsInRange(runnerUp.value().y,start.y,end.y) && GE_IsInRange(runnerUp.value().y,lastSpot->y,spot->y))
				{
					if(!currentShortest.has_value())
					{
						currentShortest = runnerUp;
					}
					else
					{
						currentShortest = {GE_ClosestVector(start,runnerUp.value(),currentShortest.value())};
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
			GE_Rectangler x = obj->collisionRectangles[i];
			GE_Rectangle y = {x.x+obj->position.x,x.y+obj->position.y,x.w,x.h};
			//GE_DEBUG_DrawRect(y);
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
