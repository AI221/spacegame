/*!
 * @file
 * @author Jackson McNeill
 *
 * The vector2 and vector2r structures, as well as math functions to go along with them. One of the building blocks of the engine.
 */
//TODO: Name-change due to scope of this file change.
#pragma once

#include <cmath>
#include <type_traits>

//Local includes
#include "GeneralEngineCPP.h"
#include "serialize.h"

#define RAD_TO_DEG 57.2957795130823231109784554604402728728018701076507568359375
#define TWO_PI 6.283185307179586

/*!
  *Prevents rotation from being >2pi
  */
GE_FORCE_INLINE constexpr void GE_CapRotation(double* rotation)
{
	(*rotation) -= floor(*rotation/TWO_PI)*TWO_PI;
}
GE_FORCE_INLINE constexpr double GE_CapRotation(double rotation)
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
	/*
	Vector2(double x, double y)
	{
		this->x=x;
		this->y=y;
	}

	template<class XY>
	Vector2(XY vector)
	{
		if constexpr(std::is_pointer<XY>::value)
		{
			this->x = vector->x;
			this->y = vector->y;
		}
		else
		{
			this->x = vector.x;
			this->y = vector.y;
		}
	}
	*/
	GE_FORCE_INLINE constexpr Vector2 operator+(double other) const 
	{
		Vector2 newVector = {this->x+other,this->y+other};
		return newVector;
	}
	GE_FORCE_INLINE constexpr Vector2 operator-(double other) const
	{
		Vector2 newVector = {this->x-other,this->y-other};
		return newVector;
	}
	GE_FORCE_INLINE constexpr Vector2 operator*(double other) const
	{
		Vector2 newVector = {this->x*other,this->y*other};
		return newVector;
	}
	GE_FORCE_INLINE constexpr Vector2 operator/(double other) const
	{
		Vector2 newVector = {this->x/other,this->y/other};
		return newVector;
	}
	template<class XY>
	GE_FORCE_INLINE constexpr Vector2 operator+(XY other) const
	{
		Vector2 newVector = {this->x+other.x,this->y+other.y};
		return newVector;
	}
	template<class XY>
	GE_FORCE_INLINE constexpr Vector2 operator-(XY other) const
	{
		Vector2 newVector = {this->x-other.x,this->y-other.y};
		return newVector;
	}
	template<class XY>
	GE_FORCE_INLINE constexpr Vector2 operator*(XY other) const
	{	
		Vector2 newVector = {this->x*other.x,this->y*other.y};
		return newVector;
	}
	GE_FORCE_INLINE void abs()
	{
		x = std::abs(x);
		y = std::abs(y);
	}
	template<class XY>
	GE_FORCE_INLINE constexpr bool operator>(XY other) const
	{
		return ( (x > other.x) && (y > other.y) );
	}
	template<class XY>
	GE_FORCE_INLINE constexpr bool operator<(XY other) const
	{
		return ( (x < other.x) && (y < other.y) );
	}
	template<class XY>
	GE_FORCE_INLINE constexpr bool operator>=(XY other) const
	{
		return ( (x >= other.x) && (y >= other.y) );
	}
	template<class XY>
	GE_FORCE_INLINE constexpr bool operator<=(XY other) const
	{
		return ( (x <= other.x) && (y <= other.y) );
	}
	template <class XY>
	GE_FORCE_INLINE constexpr Vector2 operator=(XY other) const
	{
		std::swap(x,other.x);
		std::swap(y,other.y);
		return *this;

	}
	template<class XY>
	GE_FORCE_INLINE constexpr bool operator==(XY other) const
	{
		return ( (x == other.x) && (y == other.y) );
	}

	//Vector2 operator=(const IntVector2& subject);
	

	GE_FORCE_INLINE constexpr static void serialize(Vector2* serializeMe, serialization::serialization_state& state)
	{
		serialization::serialize(serializeMe->x,state);
		serialization::serialize(serializeMe->y,state);
	}


	static Vector2* unserialize(serialization::unserialization_state& state)
	{
		auto x = serialization::unserialize<double>(state);
		auto y = serialization::unserialize<double>(state);
		return new Vector2{x,y};
	}

};





GE_FORCE_INLINE constexpr Vector2 reverseVector(Vector2 subject)
{
	auto _x = subject.x;
	subject.x = subject.y;
	subject.y = _x;
	return subject;
}

class Vector2r;
constexpr void GE_Vector2RotationCCW(Vector2r& subject);

/*!
 * A point in 2D space with rotation
 */
struct Vector2r
{
	double x;
	double y;
	double r;

	/*
	Vector2r(double x, double y,double r)
	{
		this->x=x;
		this->y=y;
		this->r=r;
	}
	Vector2r() {} //allow creation of empty vector
	
	*/

	constexpr void addRelativeVelocity(Vector2r adder);

	GE_FORCE_INLINE constexpr Vector2r operator+(Vector2r other) const
	{
		Vector2r newVector = {this->x+other.x,this->y+other.y,this->r+other.r};
		GE_CapRotation(&newVector.r);

		return newVector;
	}
	GE_FORCE_INLINE constexpr Vector2r operator-(Vector2r other) const
	{
		Vector2r newVector = {this->x-other.x,this->y-other.y,this->r-other.r};
		GE_CapRotation(&newVector.r);

		return newVector;
	}
	GE_FORCE_INLINE constexpr Vector2r operator*(Vector2r other) const
	{	
		Vector2r newVector = {this->x*other.x,this->y*other.y,this->r*other.r};
		GE_CapRotation(&newVector.r);

		return newVector;
	}
	GE_FORCE_INLINE constexpr Vector2r operator/(Vector2r other) const
	{	
		Vector2r newVector = {this->x/other.x,this->y/other.y,this->r/other.r};
		GE_CapRotation(&newVector.r);

		return newVector;
	}
	GE_FORCE_INLINE constexpr Vector2r operator*(double other) const
	{
		Vector2r newVector = {this->x*other,this->y*other,this->r*other};
		GE_CapRotation(&newVector.r);

		return newVector;
	}
	GE_FORCE_INLINE constexpr Vector2r operator/(double other) const
	{
		Vector2r newVector = {this->x/other,this->y/other,this->r/other};
		GE_CapRotation(&newVector.r);

		return newVector;
	}
	//TODO: Broken
	/*
	GE_FORCE_INLINE Vector2r operator+=(Vector2r other)
	{
		return operator+(other);
	}
	GE_FORCE_INLINE Vector2r operator-=(Vector2r other)
	{
		return operator-(other);
	}
	*/
	GE_FORCE_INLINE void abs()
	{
		x = std::abs(x);
		y = std::abs(y);
		r = std::abs(r);
	}

	//Vector2 operations

	GE_FORCE_INLINE constexpr Vector2r operator+(Vector2 other) const
	{
		Vector2r newVector = {this->x+other.x,this->y+other.y,this->r};
		return newVector;
	}
	GE_FORCE_INLINE constexpr Vector2r operator-(Vector2 other) const
	{
		Vector2r newVector = {this->x-other.x,this->y-other.y,this->r};
		return newVector;
	}
	GE_FORCE_INLINE constexpr Vector2r operator*(Vector2 other) const
	{	
		Vector2r newVector = {this->x*other.x,this->y*other.y,this->r};
		return newVector;
	}
	template <class XY>
	GE_FORCE_INLINE constexpr Vector2r operator=(XY other) const
	{
		/*if constexpr(std::is_pointer<XY>::value) //TODO: this doesn't work?
		{
			x = other->x;
			y = other->y;
			r = other->r;
		}
		else
		{
		*/
		
		std::swap(x,other.x);
		std::swap(y,other.y);
		std::swap(r,other.r);
		return *this;

	}
	template<class XYR>
	GE_FORCE_INLINE constexpr bool operator==(XYR other) const
	{
		return ( (x == other.x) && (y == other.y) && (r == other.r) );
	}
		
	GE_FORCE_INLINE constexpr static void serialize(Vector2r* data, serialization::serialization_state& state)
	{
		serialization::serialize(data->x,state);
		serialization::serialize(data->y,state);
		serialization::serialize(data->r,state);
	}
	static Vector2r* unserialize(serialization::unserialization_state& state)
	{
		double _x = serialization::unserialize<double>(state);
		double _y = serialization::unserialize<double>(state);
		double _r = serialization::unserialize<double>(state);
		return new Vector2r{_x,_y,_r};
	}

};





GE_FORCE_INLINE constexpr Vector2 GE_StripVectorRotation(Vector2r vector_subject)
{
	return Vector2{vector_subject.x,vector_subject.y};
}

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
	template<class XY>
	GE_FORCE_INLINE bool operator==(XY other)
	{
		return ( (x == other.x) && (y == other.y) );
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
struct GE_Rectangle 
{
	double x;
	double y;
	double w;
	double h;

	GE_Rectangle operator=(GE_Rectangle other)
	{
		x = other.x;
		y = other.y;
		w = other.w;
		h = other.h;
		return *this;
	}

	GE_FORCE_INLINE constexpr static void serialize(GE_Rectangle* data, serialization::serialization_state& state)
	{
		serialization::serialize(data->x,state);
		serialization::serialize(data->y,state);
		serialization::serialize(data->w,state);
		serialization::serialize(data->h,state);
	}

	static GE_Rectangle* unserialize(serialization::unserialization_state& state)
	{
		double _x = serialization::unserialize<double>(state);
		double _y = serialization::unserialize<double>(state);
		double _w = serialization::unserialize<double>(state);
		double _h = serialization::unserialize<double>(state);
		return new GE_Rectangle{_x,_y,_w,_h};
	}


};


struct GE_Rectangler 
{
	
	double x;
	double y;
	double r;
	double w;
	double h;
	
	GE_Rectangler operator=(GE_Rectangler other)
	{
		x = other.x;
		y = other.y;
		r = other.r;
		w = other.w;
		h = other.h;
		return *this;
	}


	GE_FORCE_INLINE constexpr static void serialize(GE_Rectangler* data, serialization::serialization_state& state)
	{
		serialization::serialize(data->x,state);
		serialization::serialize(data->y,state);
		serialization::serialize(data->r,state);
		serialization::serialize(data->w,state);
		serialization::serialize(data->h,state);
	}
	static GE_Rectangler* unserialize(serialization::unserialization_state& state)
	{
		double x = serialization::unserialize<double>(state);
		double y = serialization::unserialize<double>(state);
		double r = serialization::unserialize<double>(state);
		double w = serialization::unserialize<double>(state);
		double h = serialization::unserialize<double>(state);
		return new GE_Rectangler{x,y,r,w,h};
	}


};



/*!
 * Adds counter-clockwise rotation to x and y using rotation
 */
GE_FORCE_INLINE constexpr void GE_Vector2RotationCCW(double* x, double* y, double rotation)
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
GE_FORCE_INLINE constexpr void GE_Vector2RotationCCW(Vector2r* subject)
{
	GE_Vector2RotationCCW(&subject->x,&subject->y,subject->r);
}

/*!
 * Adds counter-clockwise rotation to subject's x and y using rotation
 */
GE_FORCE_INLINE constexpr void GE_Vector2RotationCCW(Vector2r* subject, double rotation)
{
	GE_Vector2RotationCCW(&subject->x,&subject->y,rotation);
}

/*!
 * Adds counter-clockwise rotation to subject's x and y using rotation
 */
GE_FORCE_INLINE constexpr void GE_Vector2RotationCCW(Vector2* subject, double rotation)
{
	GE_Vector2RotationCCW(&subject->x,&subject->y,rotation);
}


/*!
 * Adds clockwise rotation to subject's x and y using subject's r
 */
GE_FORCE_INLINE constexpr void GE_Vector2Rotation(double* x, double* y, double rotation)
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
GE_FORCE_INLINE constexpr void GE_Vector2Rotation(Vector2r* subject)
{
	GE_Vector2Rotation(&subject->x,&subject->y,subject->r);
}

/*!
 * Adds clockwise rotation to subject's x and y using subject's r
 */
GE_FORCE_INLINE constexpr void GE_Vector2Rotation(Vector2r* subject, double rotation)
{
	GE_Vector2Rotation(&subject->x,&subject->y,rotation);
}

/*!
 * Adds clockwise rotation to subject's x and y using subject's r
 */
GE_FORCE_INLINE constexpr void GE_Vector2Rotation(Vector2* subject, double rotation)
{
	GE_Vector2Rotation(&subject->x,&subject->y,rotation);
}

/*!
 * Distance between x1, y1 and x2, y2
 * @return The distance between x1, y1 and x2, y2, in a double
 */
GE_FORCE_INLINE constexpr double GE_Distance(double x1, double y1, double x2, double y2)
{
	return std::sqrt((std::pow(x2-x1,2))+(std::pow(y2-y1,2)));
}

/*!
 * Distance between 2 vectors
 * @return The distance between the 2 vectors, in a double
 */
template <class XY1,class XY2>
GE_FORCE_INLINE constexpr double GE_Distance(XY1 subject, XY2 subject2)
{
	return GE_Distance(subject.x,subject.y,subject2.x,subject2.y);
}



/*!
* Returns the result of a dot product with subject and subject2
*/
template<class XY>
GE_FORCE_INLINE constexpr double GE_Dot(XY subject, XY subject2)
{
	return (subject.x*subject2.x)+(subject.y*subject2.y);
}

/*!
 * Gives you the angle between a point with rotation, and a point
 */
template<class XYR, class XY>
GE_FORCE_INLINE constexpr double GE_GetRotationalDistance(XYR subject, XY victim)
{
	//based on: https://gamedev.stackexchange.com/a/124803 though it's pretty simple math, my major impedence was getting this done in a game jam after being up for 24+ hours
	return GE_CapRotation( (subject.r)-((atan2(victim.x-subject.x,victim.y-subject.y))) );
}




/*!
 * Converts the rotation from radians to degrees
 */
constexpr void GE_PhysicsVectorToRenderVector(Vector2r* subject)
{	
	subject->r *= RAD_TO_DEG; //SDL uses degrees instead of radians
}


/*!
 * Converts from radians to degrees
 */
constexpr void GE_PhysicsRotationToRenderRotation(double* rotation)
{
	(*rotation) = (*rotation)*RAD_TO_DEG;
}



template<class XY>
constexpr XY GE_ClosestVector(XY origin,XY point1,XY point2)
{
	if (GE_Distance(origin,point1) < GE_Distance(origin,point2))
	{
		return point1;
	}
	return point2;
}
template<class XY>
constexpr XY GE_FurthestVector(XY origin,XY point1,XY point2)
{
	if (GE_Distance(origin,point1) > GE_Distance(origin,point2))
	{
		return point1;
	}
	return point2;
}

bool GE_TEST_Vector2();



//Vector2 GE_PointsToRectangle(Vector2 start, Vector2 end,double rotation);

constexpr Vector2 GE_PointsToRectangle(Vector2 start, Vector2 end,double rotation)
{
	end = end-start;
	start = {0,0};

	Vector2 centerPoint = (start+end)/2;

	start = start-(centerPoint/2);
	end = end-(centerPoint/2);
	GE_Vector2Rotation(&start,rotation);
	GE_Vector2Rotation(&end,rotation);
	start = start+(centerPoint/2);
	end = end+(centerPoint/2);

	return {end.x-start.x,end.y-start.y};
}


constexpr void Vector2r::addRelativeVelocity(Vector2r adder)
{
	GE_Vector2RotationCCW(&adder);
	x += adder.x;
	y += adder.y;
}
