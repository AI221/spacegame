/*!
 * @file
 * @author Jackson McNeill
 * Creates y=mx+b lines, and finds their intersection points.
 */

#pragma once

#include <vector>
#include <optional>

#include "vector2.h"
#include "GeneralEngineCPP.h"


typedef std::vector<Vector2> GE_ShapeLines;
typedef std::vector<GE_ShapeLines> GE_ShapeLinesVector;


/*!
 * A line.
 * If vertical, then x=b. Otherwise, y=mx+b
 */
struct GE_Line
{
	double m;
	double b;
	bool vertical;
};

constexpr GE_Line GE_GetLine(Vector2 start, Vector2 end);

constexpr std::optional<double> GE_LineIntersection(double m1,double b1,double m2,double b2,unsigned char infValue);


constexpr std::optional<Vector2> GE_LineIntersectionVector(GE_Line line_one, GE_Line line_two);
