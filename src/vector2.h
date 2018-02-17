/*!
 * @file
 * @author Jackson McNeill
 *
 * The vector2 and vector2r structures, as well as math functions to go along with them. One of the building blocks of the engine.
 */
//TODO: Name-change due to scope of this file change.

#include <cmath>

//Local includes
#include "GeneralEngineCPP.h"

#ifndef __VECTOR2_INCLUDED
#define __VECTOR2_INCLUDED
#define RAD_TO_DEG 57.2957795130823231109784554604402728728018701076507568359375
#define TWO_PI 6.283185307179586

/*!
  *Prevents rotation from being >2pi
  */
GE_FORCE_INLINE void GE_CapRotation(double* rotation)
{
	(*rotation) -= floor(*rotation/TWO_PI)*TWO_PI;
}
GE_FORCE_INLINE double GE_CapRotation(double rotation)
{
	GE_CapRotation(&rotation);
	return rotation;
}


struct IntVector2;

/*!
 * A point in 2D space with no rotation. 
 */
struct Vector2 
{
	double x;
	double y;
	template<class XY>
	GE_FORCE_INLINE Vector2 operator+(XY other)
	{
		Vector2 newVector = {this->x+other.x,this->y+other.y};
		return newVector;
	}
	template<class XY>
	GE_FORCE_INLINE Vector2 operator-(XY other)
	{
		Vector2 newVector = {this->x-other.x,this->y-other.y};
		return newVector;
	}
	template<class XY>
	GE_FORCE_INLINE Vector2 operator*(XY other)
	{	
		Vector2 newVector = {this->x*other.x,this->y*other.y};
		return newVector;
	}
	GE_FORCE_INLINE Vector2 operator*(double other)
	{
		Vector2 newVector = {this->x*other,this->y*other};
		return newVector;
	}
	GE_FORCE_INLINE Vector2 operator/(double other)
	{
		Vector2 newVector = {this->x/other,this->y/other};
		return newVector;
	}
	GE_FORCE_INLINE void abs()
	{
		x = std::abs(x);
		y = std::abs(y);
	}
	template<class XY>
	GE_FORCE_INLINE bool operator>(XY other)
	{
		return ( (x > other.x) && (y > other.y) );
	}
	template<class XY>
	GE_FORCE_INLINE bool operator<(XY other)
	{
		return ( (x < other.x) && (y < other.y) );
	}
	template<class XY>
	GE_FORCE_INLINE bool operator>=(XY other)
	{
		return ( (x >= other.x) && (y >= other.y) );
	}
	template<class XY>
	GE_FORCE_INLINE bool operator<=(XY other)
	{
		return ( (x <= other.x) && (y <= other.y) );
	}

	//Vector2 operator=(const IntVector2& subject);
};


/*!
 * A point in 2D space with rotation
 */
struct Vector2r
{
	double x;
	double y;
	double r;

	void addRelativeVelocity(Vector2r adder);
	GE_FORCE_INLINE Vector2r operator+(Vector2r other)
	{
		Vector2r newVector = {this->x+other.x,this->y+other.y,this->r+other.r};
		GE_CapRotation(&newVector.r);

		return newVector;
	}
	GE_FORCE_INLINE Vector2r operator-(Vector2r other)
	{
		Vector2r newVector = {this->x-other.x,this->y-other.y,this->r-other.r};
		GE_CapRotation(&newVector.r);

		return newVector;
	}
	GE_FORCE_INLINE Vector2r operator*(Vector2r other)
	{	
		Vector2r newVector = {this->x*other.x,this->y*other.y,this->r*other.r};
		GE_CapRotation(&newVector.r);

		return newVector;
	}
	GE_FORCE_INLINE Vector2r operator/(Vector2r other)
	{	
		Vector2r newVector = {this->x/other.x,this->y/other.y,this->r/other.r};
		GE_CapRotation(&newVector.r);

		return newVector;
	}
	GE_FORCE_INLINE Vector2r operator*(double other)
	{
		Vector2r newVector = {this->x*other,this->y*other,this->r*other};
		GE_CapRotation(&newVector.r);

		return newVector;
	}
	GE_FORCE_INLINE Vector2r operator/(double other)
	{
		Vector2r newVector = {this->x/other,this->y/other,this->r/other};
		GE_CapRotation(&newVector.r);

		return newVector;
	}
	GE_FORCE_INLINE Vector2r operator+=(Vector2r other)
	{
		return operator+(other);
	}
	GE_FORCE_INLINE Vector2r operator-=(Vector2r other)
	{
		return operator-(other);
	}
	GE_FORCE_INLINE void abs()
	{
		x = std::abs(x);
		y = std::abs(y);
		r = std::abs(r);
	}

	//Vector2 operations

	GE_FORCE_INLINE Vector2r operator+(Vector2 other)
	{
		Vector2r newVector = {this->x+other.x,this->y+other.y,this->r};
		return newVector;
	}
	GE_FORCE_INLINE Vector2r operator-(Vector2 other)
	{
		Vector2r newVector = {this->x-other.x,this->y-other.y,this->r};
		return newVector;
	}
	GE_FORCE_INLINE Vector2r operator*(Vector2 other)
	{	
		Vector2r newVector = {this->x*other.x,this->y*other.y,this->r};
		return newVector;
	}
		
};


struct IntVector2
{
	int x;
	int y;
	template<class XY>
	GE_FORCE_INLINE IntVector2 operator+(XY other)
	{
		IntVector2 newVector = {static_cast<int>(this->x+other.x),static_cast<int>(this->y+other.y)};
		return newVector;
	}
	template<class XY>
	GE_FORCE_INLINE IntVector2 operator-(XY other)
	{
		IntVector2 newVector = {static_cast<int>(this->x-other.x),static_cast<int>(this->y-other.y)};
		return newVector;
	}
	template<class XY>
	GE_FORCE_INLINE IntVector2 operator*(XY other)
	{	
		IntVector2 newVector = {static_cast<int>(this->x*other.x),static_cast<int>(this->y*other.y)};
		return newVector;
	}
	GE_FORCE_INLINE IntVector2 operator*(double other)
	{
		IntVector2 newVector = {static_cast<int>(this->x*other),static_cast<int>(this->y*other)};
		return newVector;
	}
	GE_FORCE_INLINE IntVector2 operator/(double other)
	{
		IntVector2 newVector = {static_cast<int>(this->x/other),static_cast<int>(this->y/other)};
		return newVector;
	}
	GE_FORCE_INLINE void abs()
	{
		x = std::abs(x);
		y = std::abs(y);
	}

	template<class XY>
	GE_FORCE_INLINE bool operator>(XY other)
	{
		return ( (x > other.x) && (y > other.y) );
	}
	template<class XY>
	GE_FORCE_INLINE bool operator<(XY other)
	{
		return ( (x < other.x) && (y < other.y) );
	}
	template<class XY>
	GE_FORCE_INLINE bool operator>=(XY other)
	{
		return ( (x >= other.x) && (y >= other.y) );
	}
	template<class XY>
	GE_FORCE_INLINE bool operator<=(XY other)
	{
		return ( (x <= other.x) && (y <= other.y) );
	}


};

/*
	x=0;y=0;w=10;h=10;

	   width
	x,y-----|
	|		|
	|		|
  h |		|
  e |		|
  i |		| 
  g |		|
  h |		|
  t |		|
	|-------|<-- 10,10 (0+10,0+10)

*/


/*! 
 *	A 2D Rectangle. No additional operators provided.
 */
struct GE_Rectangle //TODO allow rectangles themselves to be rotated relative to their owners
{
	double x;
	double y;
	double w;
	double h;
};

/*!
 * Adds counter-clockwise rotation to x and y using rotation
 */
GE_FORCE_INLINE void GE_Vector2RotationCCW(double* x, double* y, double rotation)
{
	double sin_rotation = sin(rotation); //posr must be used because this vector is a velocity vector, not a position vector
	double cos_rotation = cos(rotation);

	double new_x = ((*x)*cos_rotation)+((*y)*sin_rotation); //we cannot re-assign the x-value before the other transmformation
	(*y) =	-((*x)*sin_rotation)+((*y)*cos_rotation);

	*x = new_x;

}


/*!
 * Adds counter-clockwise rotation to subject's x and y using subject's r
 */
GE_FORCE_INLINE void GE_Vector2RotationCCW(Vector2r* subject)
{
	GE_Vector2RotationCCW(&subject->x,&subject->y,subject->r);
}

/*!
 * Adds counter-clockwise rotation to subject's x and y using rotation
 */
GE_FORCE_INLINE void GE_Vector2RotationCCW(Vector2r* subject, double rotation)
{
	GE_Vector2RotationCCW(&subject->x,&subject->y,rotation);
}

/*!
 * Adds counter-clockwise rotation to subject's x and y using rotation
 */
GE_FORCE_INLINE void GE_Vector2RotationCCW(Vector2* subject, double rotation)
{
	GE_Vector2RotationCCW(&subject->x,&subject->y,rotation);
}


/*!
 * Adds clockwise rotation to subject's x and y using subject's r
 */
GE_FORCE_INLINE void GE_Vector2Rotation(double* x, double* y, double rotation)
{
	double sin_rotation = sin(rotation); //posr must be used because this vector is a velocity vector, not a position vector
	double cos_rotation = cos(rotation);

	double new_x = ((*x)*cos_rotation)-((*y)*sin_rotation); //we cannot re-assign the x-value before the other transmformation
	(*y) =	((*x)*sin_rotation)+((*y)*cos_rotation);

	*x = new_x;

}

/*!
 * Adds clockwise rotation to subject's x and y using subject's r
 */
GE_FORCE_INLINE void GE_Vector2Rotation(Vector2r* subject)
{
	GE_Vector2Rotation(&subject->x,&subject->y,subject->r);
}

/*!
 * Adds clockwise rotation to subject's x and y using subject's r
 */
GE_FORCE_INLINE void GE_Vector2Rotation(Vector2r* subject, double rotation)
{
	GE_Vector2Rotation(&subject->x,&subject->y,rotation);
}

/*!
 * Adds clockwise rotation to subject's x and y using subject's r
 */
GE_FORCE_INLINE void GE_Vector2Rotation(Vector2* subject, double rotation)
{
	GE_Vector2Rotation(&subject->x,&subject->y,rotation);
}

/*!
 * Distance between x1, y1 and x2, y2
 * @return The distance between x1, y1 and x2, y2, in a double
 */
GE_FORCE_INLINE double GE_Distance(double x1, double y1, double x2, double y2)
{
	return sqrt((pow(x2-x1,2))+(pow(y2-y1,2)));
}

/*!
 * Distance between 2 vectors
 * @return The distance between the 2 vectors, in a double
 */
template <class XY>
GE_FORCE_INLINE double GE_Distance(XY subject, XY subject2)
{
	return GE_Distance(subject.x,subject.y,subject2.x,subject2.y);
}



/*!
* Returns the result of a dot product with subject and subject2
*/
template<class XY>
GE_FORCE_INLINE double GE_Dot(XY subject, XY subject2)
{
	return (subject.x*subject2.x)+(subject.y*subject2.y);
}

template<class XYR, class XY>
GE_FORCE_INLINE double GE_GetRotationalDistance(XYR subject, XY victim)
{
	//based on: https://gamedev.stackexchange.com/a/124803 though it's pretty simple math, my major impedence was getting this done in a game jam after being up for 24+ hours
	return GE_CapRotation( (subject.r)-((atan2(victim.x-subject.x,victim.y-subject.y))) );
}



/*!
 * Converts the rotation from radians to degrees
 */
void GE_PhysicsVectorToRenderVector(Vector2r* subject);
/*!
 * Converts from radians to degrees
 */
void GE_PhysicsRotationToRenderRotation(double* rotation);

template<class XY>
XY GE_ClosestVector(XY origin,XY point1,XY point2)
{
	if (GE_Distance(origin,point1) < GE_Distance(origin,point2))
	{
		return point1;
	}
	return point2;
}
template<class XY>
XY GE_FurthestVector(XY origin,XY point1,XY point2)
{
	if (GE_Distance(origin,point1) > GE_Distance(origin,point2))
	{
		return point1;
	}
	return point2;
}

#endif //__VECTOR2_INCLUDED0.14285714285714285
