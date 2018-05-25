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



/*
Vector2 Vector2::operator=(const IntVector2& subject)
{
	x = static_cast<double>(subject.x);
	y = static_cast<double>(subject.y);
	return *this;
}*/

#define internal_stringify_vector2(v) ((std::to_string(v.x)+std::string(", ")+std::to_string(v.y)).c_str())
#define internal_stringify_vector2r(v)( (std::to_string(v.x)+std::string(", ")+std::to_string(v.y)+std::string(", ")+std::to_string(v.r)).c_str())

bool GE_TEST_Vector2()
{
	bool passedAll = true; //unit testing will automatically set this


	{
		Vector2 firstVector,secondVector,resultantVector;
		firstVector = {1.5,2};
		secondVector = {3,4};
		resultantVector = firstVector+secondVector;

		GE_TEST_Log("Ensure Vector2 adding works\n");
		GE_TEST_ASSERT(internal_stringify_vector2,resultantVector,(Vector2{4.5,6}),==);

		firstVector = {2,3};
		secondVector = {0.5,4};
		resultantVector = firstVector*secondVector;

		GE_TEST_Log("Ensure Vector2 multiplication works\n");
		GE_TEST_ASSERT(internal_stringify_vector2,resultantVector,(Vector2{1,12}),==);


		resultantVector = (Vector2{4,8})/2.0;

		GE_TEST_Log("Ensure Vector2 division against single double works\n");
		GE_TEST_ASSERT(internal_stringify_vector2,resultantVector,(Vector2{2,4}),==);

	}
	

	return passedAll;
}



