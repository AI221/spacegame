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
Vector2r positionBuffer[MAX_GLUE_TARGETS];
Vector2r velocityBuffer[MAX_GLUE_TARGETS];
GE_Rectangle gridbuffer[MAX_GLUE_TARGETS];
int countGlueTargets = -1;

pthread_mutex_t GlueMutex;

int GE_GlueInit()
{
	GE_AddPhysicsPreCallback(GE_GluePreCallback);
	GE_AddPhysicsDoneCallback(GE_GlueCallback);

	return 0;
}
void GE_GluePreCallback()
{
	//TODO mutexes
	
	GE_PhysicsObject* cObj;
	for (int i = 0; i < countGlueTargets+1; i++)
	{
		if (GE_GetPhysicsObjectFromID(targets[i].physicsObjectID,&cObj) == 0) 
		{
			if(targets[i].typeAddPosition == GE_ADD_TYPE_NORM)
			{
				GE_AddVelocity(cObj,targets[i].addPosition);
			}

			if(targets[i].typeAddVelocity == GE_ADD_TYPE_NORM)
			{
				GE_AddVelocity(cObj,targets[i].addVelocity);
			}
			else if (targets[i].typeAddVelocity == GE_ADD_TYPE_RELATIVE)
			{
				GE_AddRelativeVelocity(cObj,targets[i].addVelocity);
			}
			targets[i].typeAddVelocity = GE_ADD_TYPE_NONE;
			targets[i].typeAddPosition = GE_ADD_TYPE_NONE;
		}
	}
			
		
}
void GE_GlueCallback()
{
	//printf("try lock render\n");
	pthread_mutex_lock(&GlueMutex);
	//printf("lock render\n");
	GE_PhysicsObject* cObj;
	for (int i = 0; i < countGlueTargets+1; i++)
	{
		if (GE_GetPhysicsObjectFromID(targets[i].physicsObjectID,&cObj) == 0) //if no error from getting the ID
		{
			//(*(targets[i].subject)) = cObj->position;
			
			positionBuffer[i] = cObj->position;
			velocityBuffer[i] = cObj->velocity;
			gridbuffer[i] = cObj->grid;
			//printf("X: %f \n",targets[i].subject->x);
			
			//transfer new velocities/positions
			

		}
		else
		{
			//printf("[temp err - handle] failed to get po from id\n");
		}
		
	}
	pthread_mutex_unlock(&GlueMutex);
	//printf("fin glue\n");
}
void GE_GlueRenderCallback()
{
	pthread_mutex_lock(&GlueMutex);
	for (int i = 0; i < countGlueTargets+1; i++)
	{
		(*(targets[i].subject)) = positionBuffer[i];
	}
	pthread_mutex_unlock(&GlueMutex);
}
int GE_addGlueSubject(Vector2r* subject, int physicsID)
{
	GE_NoGreaterThan(countGlueTargets,MAX_GLUE_TARGETS);
	targets[countGlueTargets+1] = GE_GlueTarget{subject, physicsID,NULL,GE_ADD_TYPE_NONE,NULL,GE_ADD_TYPE_NONE};
	countGlueTargets++;
	return 0;
}

void GE_glueAddVelocity(int targetID, Vector2r ammount, GE_ADD_TYPE type)
{
	targets[targetID].addVelocity = ammount;
	targets[targetID].typeAddVelocity = type;
}

void GE_glueAddPosition(int targetID, Vector2r ammount, GE_ADD_TYPE type)
{
	targets[targetID].addPosition = ammount;
	targets[targetID].typeAddPosition = type;
}
