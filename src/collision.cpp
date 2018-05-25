#include "collision.h"
#include <array>

struct line_plus_range
{
	GE_Line line;
	Vector2 start;
	Vector2 end;
};
typedef std::vector<line_plus_range> shape_lines_vector;
shape_lines_vector get_shape_lines(GE_ShapePoints input)
{
	shape_lines_vector result;
	result.reserve(input.size());
	Vector2 lastPoint = *std::begin(input);
	for (auto it=std::begin(input)+1;it != std::end(input);it++)
	{
		result.push_back(line_plus_range{GE_GetLine(lastPoint,*it),lastPoint,*it});
		lastPoint = *it;
	}
	return result;
}
constexpr bool point_in_range_of_line(Vector2 point, line_plus_range line)
{
	return GE_IsInRange(point.x,line.start.x,line.end.x) && GE_IsInRange(point.y,line.start.y,line.end.y);
}
constexpr bool collision_in_range_of_lines(Vector2 collision,line_plus_range first,line_plus_range second)
{
	return point_in_range_of_line(collision,first) && point_in_range_of_line(collision,second);

}

bool GE_ShapeCollision(GE_ShapePoints shape1, GE_ShapePoints shape2)
{
	shape_lines_vector shapelines1 = get_shape_lines(shape1);
	shape_lines_vector shapelines2 = get_shape_lines(shape2);
	for (auto line1_plus_range = std::begin(shapelines1);line1_plus_range!= std::end(shapelines1);line1_plus_range++)
	{
		for (auto line2_plus_range = std::begin(shapelines2);line2_plus_range!= std::end(shapelines2);line2_plus_range++)
		{
			std::optional<Vector2> collisionPoint = GE_LineIntersectionVector(line1_plus_range->line,line2_plus_range->line);
			if (collisionPoint.has_value())
			{
				if (collision_in_range_of_lines(collisionPoint.value(),*line1_plus_range,*line2_plus_range))
				{
					return true;
				}
			}
		}
	}
	return false;
}


