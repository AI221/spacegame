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

GE_GlueTarget targets[MAX_GLUE_TARGETS];
int countGlueTargets = -1;

#include<SDL2/SDL.h>
int GE_GlueInit()
{
	GE_AddPhysicsDoneCallback(GE_GlueCallback);
	return 0;
}
void GE_GlueCallback()
{
	pthread_mutex_lock(&RenderEngineMutex);
	GE_PhysicsObject* cObj;
	for (int i = 0; i < countGlueTargets+1; i++)
	{
		if (GE_GetPhysicsObjectFromID(targets[i].physicsObjectID,&cObj) == 0) //if no error from getting the ID
		{
			(*(targets[i].subject)) = cObj->position;
			//printf("X: %f \n",targets[i].subject->x);
		}
		else
		{
			printf("[temp err - handle] failed to get po from id\n");
		}
		
	}
	pthread_mutex_unlock(&RenderEngineMutex);
}
int GE_addGlueSubject(Vector2r* subject, int physicsID)
{
	GE_NoGreaterThan(countGlueTargets,MAX_GLUE_TARGETS);
	targets[countGlueTargets+1] = GE_GlueTarget{subject, physicsID};
	countGlueTargets++;
	return 0;
}

