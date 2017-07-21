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
void* GE_glueThreadMain(void* )
{
	syncWithPhysicsEngine = true;
	printf("I'm here!");
	while(true)
	{
		if (physicsEngineDone)
		{
			pthread_mutex_lock(&PhysicsEngineMutex);
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                            glueThread\n");
			
			for (int i = 0; i < countGlueTargets+1; i++)
			{
				(*(targets[i].subject)) = allPhysicsObjects[targets[i].physicsObjectID]->position;
				printf("X: %f \n",targets[i].subject->x);
			}
			pthread_mutex_unlock(&PhysicsEngineMutex);
			physicsEngineDone = false;
		}
	}
}
void GE_addGlueSubject(Vector2r* subject, int physicsID)
{
	//TODO: is this okay to do whilst glueThreadMain is running?
	countGlueTargets++;

	targets[countGlueTargets] = glueTarget{subject, physicsID};
}

