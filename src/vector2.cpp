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
void Vector2r::addRelativeVelocity(Vector2r adder)
{
	GE_Vector2RotationCCW(&adder);
	x += adder.x;
	y += adder.y;
}
void GE_PhysicsVectorToRenderVector(Vector2r* subject)
{	
	subject->r *= RAD_TO_DEG; //SDL uses degrees instead of radians
}
void GE_PhysicsRotationToRenderRotation(double* rotation)
{
	(*rotation) = (*rotation)*RAD_TO_DEG;
}

/*
Vector2 Vector2::operator=(const IntVector2& subject)
{
	x = static_cast<double>(subject.x);
	y = static_cast<double>(subject.y);
	return *this;
}*/
