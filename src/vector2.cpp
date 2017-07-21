/*
Copyright 2017 Jackson Reed McNeill

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "vector2.h" 

void GE_Vector2RotationCCW(Vector2r* subject)
{
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
	(*y) =	-((*x)*sin_rotation)+((*y)*cos_rotation);

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
	(*y) =	((*x)*sin_rotation)+((*y)*cos_rotation);

	*x = new_x;

}

double GE_Distance(Vector2 subject, Vector2 subject2)
{
	return GE_Distance(subject.x,subject.y,subject2.x,subject2.y);
}

double GE_Distance(double x1, double y1, double x2, double y2) 
{
	return sqrt((pow(x2-x1,2))+(pow(y2-y1,2)));
}

void Vector2r::addRelativeVelocity(Vector2r adder)
{
	GE_Vector2RotationCCW(&adder);
	x += adder.x;
	y += adder.y;
}
