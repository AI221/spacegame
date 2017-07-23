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

#include "gluePhysicsObject.h"

glueTarget targets[MAX_GLUE_TARGETS];
int countGlueTargets = -1;

#include<SDL2/SDL.h>
int GE_GlueInit()
{
	GE_AddPhysicsDoneCallback(GE_GlueCallback);
}
void GE_GlueCallback()
{
	pthread_mutex_lock(&PhysicsEngineMutex);
	printf("PE Mutex locked\n");
	pthread_mutex_lock(&RenderEngineMutex);
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!							  glueThread\n");
	
	PhysicsObject* cObj;
	for (int i = 0; i < countGlueTargets+1; i++)
	{
		printf("1\n");
		if (GE_GetPhysicsObjectFromID(targets[i].physicsObjectID,&cObj) == 0) //if no error from getting the ID
		{
			(*(targets[i].subject)) = cObj->position;
			printf("X: %f \n",targets[i].subject->x);
		}
		else
		{
			printf("[temp err - handle] failed to get po from id\n");
		}
		printf("2\n");
		
	}
	pthread_mutex_unlock(&RenderEngineMutex);
	pthread_mutex_unlock(&PhysicsEngineMutex);
}
void GE_addGlueSubject(Vector2r* subject, int physicsID)
{
	//TODO: is this okay to do whilst glueThreadMain is running?
	countGlueTargets++;

	targets[countGlueTargets] = glueTarget{subject, physicsID};
}

