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
	void addRelativeVelocity(double inx, double iny,double posr)
	{
		double sin_rotation = sin(posr/DEG_TO_RAD); //posr must be used because this vector is a velocity vector, not a position vector
		double cos_rotation = cos(posr/DEG_TO_RAD);
		double new_x = (inx*cos_rotation)+(iny*sin_rotation);
		double new_y = -(inx*sin_rotation)+(iny*cos_rotation);
		/*std::cout << "r " << posr << std::endl;
		std::cout << "cin " << sin_rotation << std::endl;
		std::cout << "cos " << cos_rotation << std::endl;
		std::cout << "new_x " << new_x << std::endl;
		std::cout << "new_y " << new_y << std::endl;*/
		x += new_x;
		y += new_y;
	}
	Vector2r operator+(Vector2r other)
	{
		Vector2r newVector = {this->x+other.x,this->y+other.y,this->r+other.r};
		//prevent rotation from being >360
		newVector.r -= floor(newVector.r/360)*360;

		return newVector;
	}
	Vector2r operator+=(Vector2r other)
	{
		return operator+(other);
	}
		
};

#endif //__VECTOR2_INCLUDED
