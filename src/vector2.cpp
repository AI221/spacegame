#include "vector2.h" 

void GE_Vector2RotationCCW(Vector2r* subject)
{
	/*double sin_rotation = sin(subject->r/DEG_TO_RAD); //posr must be used because this vector is a velocity vector, not a position vector
	double cos_rotation = cos(subject->r/DEG_TO_RAD);

	double new_x = (subject->x*cos_rotation)+(subject->y*sin_rotation); //we cannot re-assign the x-value before the other transmformation
	subject->y = -(subject->x*sin_rotation)+(subject->y*cos_rotation);

	subject->x = new_x;*/

	GE_Vector2RotationCCW(&subject->x,&subject->y,subject->r);

}

void GE_Vector2RotationCCW(Vector2r* subject, double rotation)
{
	GE_Vector2RotationCCW(&subject->x,&subject->y,rotation);
}
void GE_Vector2RotationCCW(Vector2* subject, double rotation)
{
	GE_Vector2RotationCCW(&subject->x,&subject->y,rotation);
}

void GE_Vector2RotationCCW(double* x, double* y, double rotation)
{
	double sin_rotation = sin(rotation/DEG_TO_RAD); //posr must be used because this vector is a velocity vector, not a position vector
	double cos_rotation = cos(rotation/DEG_TO_RAD);

	double new_x = ((*x)*cos_rotation)+((*y)*sin_rotation); //we cannot re-assign the x-value before the other transmformation
	(*y) =  -((*x)*sin_rotation)+((*y)*cos_rotation);

	*x = new_x;

}



void GE_Vector2Rotation(Vector2r* subject)
{
	GE_Vector2Rotation(&subject->x,&subject->y,subject->r);
}
void GE_Vector2Rotation(Vector2r* subject, double rotation)
{
	GE_Vector2Rotation(&subject->x,&subject->y,rotation);
}
void GE_Vector2Rotation(Vector2* subject, double rotation)
{
	GE_Vector2Rotation(&subject->x,&subject->y,rotation);
}

void GE_Vector2Rotation(double* x, double* y, double rotation)
{
	double sin_rotation = sin(rotation/DEG_TO_RAD); //posr must be used because this vector is a velocity vector, not a position vector
	double cos_rotation = cos(rotation/DEG_TO_RAD);

	double new_x = ((*x)*cos_rotation)-((*y)*sin_rotation); //we cannot re-assign the x-value before the other transmformation
	(*y) =  ((*x)*sin_rotation)+((*y)*cos_rotation);

	*x = new_x;

}

void Vector2r::addRelativeVelocity(Vector2r adder)
{
	
	
	GE_Vector2RotationCCW(&adder);
	
	

	x += adder.x;
	y += adder.y;
}
