/*!
 * @file
 * @author Jackson McNeill
 *
 * The vector2 and vector2r structures, as well as math functions to go along with them. One of the building blocks of the engine.
 */
//TODO: Name-change due to scope of this file change.

#include <math.h>

#ifndef __VECTOR2_INCLUDED
#define __VECTOR2_INCLUDED
#define DEG_TO_RAD 57.2958


/*!
 * A point in 2D space with no rotation. 
 */
struct Vector2 
{
	double x;
	double y;
};


/*!
 * A point in 2D space with rotation
 */
struct Vector2r
{
	double x;
	double y;
	double r;

	/*
	 * Deprecated
	 */
	void addRelativeVelocity(Vector2r adder);
	Vector2r operator+(Vector2r other)
	{
		Vector2r newVector = {this->x+other.x,this->y+other.y,this->r+other.r};
		//prevent rotation from being >360
		newVector.r -= floor(newVector.r/360)*360;

		return newVector;
	}
	Vector2r operator-(Vector2r other)
	{
		Vector2r newVector = {this->x-other.x,this->y-other.y,this->r-other.r};
		//prevent rotation from being >360
		newVector.r -= floor(newVector.r/360)*360;

		return newVector;
	}
	Vector2r operator*(Vector2r other)
	{	
		Vector2r newVector = {this->x*other.x,this->y*other.y,this->r*other.r};
		newVector.r -= floor(newVector.r/360)*360;

		return newVector;
	}
	Vector2r operator*(int other)
	{
		Vector2r newVector = {this->x*other,this->y*other,this->r*other};
		newVector.r -= floor(newVector.r/360)*360;

		return newVector;
	}
	Vector2r operator+=(Vector2r other)
	{
		return operator+(other);
	}
	Vector2r operator-=(Vector2r other)
	{
		return operator-(other);
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
 * Adds counter-clockwise rotation to subject's x and y using subject's r
 */
void GE_Vector2RotationCCW(Vector2r* subject);

/*!
 * Adds counter-clockwise rotation to subject's x and y using rotation
 */
void GE_Vector2RotationCCW(Vector2r* subject, double rotation);

/*!
 * Adds counter-clockwise rotation to subject's x and y using rotation
 */
void GE_Vector2RotationCCW(Vector2* subject, double rotation);

/*!
 * Adds counter-clockwise rotation to x and y using rotation
 */
void GE_Vector2RotationCCW(double* x, double* y, double rotation);



/*!
 * Adds clockwise rotation to subject's x and y using subject's r
 */
void GE_Vector2Rotation(Vector2r* subject);

/*!
 * Adds clockwise rotation to subject's x and y using subject's r
 */
void GE_Vector2Rotation(Vector2r* subject, double rotation);

/*!
 * Adds clockwise rotation to subject's x and y using subject's r
 */
void GE_Vector2Rotation(Vector2* subject, double rotation);

/*!
 * Adds clockwise rotation to subject's x and y using subject's r
 */
void GE_Vector2Rotation(double* x, double* y, double rotation);

/*!
 * Distance between 2 vectors
 * @return The distance between the 2 vectors, in a double
 */
double GE_Distance(Vector2 subject, Vector2 subject2);

/*!
 * Distance between x1, y1 and x2, y2
 * @return The distance between x1, y1 and x2, y2, in a double
 */
double GE_Distance(double x1, double y1, double x2, double y2);




#endif //__VECTOR2_INCLUDED
