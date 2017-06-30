#include <math.h>

#ifndef __VECTOR2_INCLUDED
#define __VECTOR2_INCLUDED
#define DEG_TO_RAD 57.2958
struct Vector2
{
	double x;
	double y;
};
struct Vector2r
{
	double x;
	double y;
	double r;
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

void GE_Vector2RotationCCW(Vector2r* subject);
void GE_Vector2RotationCCW(Vector2r* subject, double rotation);
void GE_Vector2RotationCCW(Vector2* subject, double rotation);
void GE_Vector2RotationCCW(double* x, double* y, double rotation);

void GE_Vector2Rotation(Vector2r* subject);
void GE_Vector2Rotation(Vector2r* subject, double rotation);
void GE_Vector2Rotation(Vector2* subject, double rotation);
void GE_Vector2Rotation(double* x, double* y, double rotation);


#endif //__VECTOR2_INCLUDED
