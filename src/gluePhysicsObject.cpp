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

GE_GlueTarget* targets[MAX_GLUE_TARGETS];
bool deadTargets[MAX_GLUE_TARGETS];
int countGlueTargets = -1;

pthread_mutex_t GlueMutex;


void GE_GluePreCallback()
{
	//TODO mutexes
	pthread_mutex_lock(&GlueMutex);
	
	for (int i = 0; i < countGlueTargets+1; i++)
	{
		if (!deadTargets[i])
		{
			if (targets[i]->pullOn == GE_PULL_ON_RENDER)
			{
				memcpy(targets[i]->updateData,targets[i]->buffer,targets[i]->sizeOfPullData); //Copy from the buffer to the updated value
			}
		}
	}
	pthread_mutex_unlock(&GlueMutex);
}
void GE_GlueCallback()
{
	//printf("try lock render\n");
	pthread_mutex_lock(&GlueMutex);
	//printf("lock render\n");
	for (int i = 0; i < countGlueTargets+1; i++)
	{
		if (!deadTargets[i])
		{
			if (targets[i]->pullOn == GE_PULL_ON_PHYSICS_TICK)
			{
				memcpy(targets[i]->buffer,targets[i]->pullData,targets[i]->sizeOfPullData);//Copy to the buffer the pulled data
			}
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
		if (!deadTargets[i])
		{
			if (targets[i]->pullOn == GE_PULL_ON_PHYSICS_TICK)
			{
				memcpy(targets[i]->updateData,targets[i]->buffer,targets[i]->sizeOfPullData);//Copy to the update data from the buffer
			}
			else if (targets[i]->pullOn == GE_PULL_ON_RENDER)
			{
				memcpy(targets[i]->buffer,targets[i]->pullData,targets[i]->sizeOfPullData);//Copy to the buffer the pulled data
			}
		}

	}
	pthread_mutex_unlock(&GlueMutex);
}
GE_GlueTarget* GE_addGlueSubject(void* updateData, void* pullData, GE_PULL_ON pullOn, size_t sizeOfPullData)
{
	GE_NoGreaterThan_NULL(countGlueTargets,MAX_GLUE_TARGETS);
	void* bufferAlloc = operator new(sizeOfPullData);//allocate a buffer of size sizeOfPullData to store the data in between taking it after one source runs and copying it before another one runs


	GE_GlueTarget* newGlue = new GE_GlueTarget{updateData, pullData,pullOn,sizeOfPullData,bufferAlloc,countGlueTargets};
	memcpy(newGlue->buffer,newGlue->pullData,newGlue->sizeOfPullData);//Copy to the buffer the pulled data - we need to do this to avoid potentially writing an unitialized value to the buffer
	targets[countGlueTargets+1] = newGlue;
	countGlueTargets++; //NOTE: This is assumed to be an atomic operation and thus be thread-safe to write while other threads are reading. This is true for x86.
	return newGlue;
}

void GE_FreeGlueObject(GE_GlueTarget* subject)
{
	pthread_mutex_lock(&GlueMutex);
	deadTargets[subject->ID] = true;
	delete (char*) subject->buffer;
	delete subject;
	pthread_mutex_unlock(&GlueMutex);
}
