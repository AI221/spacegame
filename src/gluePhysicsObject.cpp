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

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <list>

#include "vector2.h"
#include "GeneralEngineCPP.h"

struct GE_GlueTarget
{
	void* updateData;
	void* pullData;
	GE_PULL_ON pullOn;
	size_t sizeOfPullData;

	void* buffer;
};


std::list<GE_GlueTarget*> targets;


pthread_mutex_t GlueMutex = PTHREAD_MUTEX_INITIALIZER;


void GE_GluePreCallback()
{
	//TODO mutexes
	pthread_mutex_lock(&GlueMutex);
	
	for (GE_GlueTarget* target : targets)
	{
		if (target->pullOn == GE_PULL_ON_RENDER)
		{
			memcpy(target->updateData,target->buffer,target->sizeOfPullData); //Copy from the buffer to the updated value
		}
	}
	pthread_mutex_unlock(&GlueMutex);
}
void GE_GlueCallback()
{
	//printf("try lock render\n");
	pthread_mutex_lock(&GlueMutex);
	//printf("lock render\n");
	for (GE_GlueTarget* target : targets)
	{
		if (target->pullOn == GE_PULL_ON_PHYSICS_TICK)
		{
			memcpy(target->buffer,target->pullData,target->sizeOfPullData);//Copy to the buffer the pulled data
		}
		
	}
	pthread_mutex_unlock(&GlueMutex);
	//printf("fin glue\n");
}
void GE_GlueRenderCallback()
{
	pthread_mutex_lock(&GlueMutex);
	for (GE_GlueTarget* target : targets)
	{
		if (target->pullOn == GE_PULL_ON_PHYSICS_TICK)
		{
			memcpy(target->updateData,target->buffer,target->sizeOfPullData);//Copy to the update data from the buffer
		}
		else if (target->pullOn == GE_PULL_ON_RENDER)
		{
			memcpy(target->buffer,target->pullData,target->sizeOfPullData);//Copy to the buffer the pulled data
		}

	}
	pthread_mutex_unlock(&GlueMutex);
}
GE_GlueTarget* GE_addGlueSubject(void* updateData, void* pullData, GE_PULL_ON pullOn, size_t sizeOfPullData)
{
	pthread_mutex_lock(&GlueMutex);
	void* bufferAlloc = malloc(sizeOfPullData);//allocate a buffer of size sizeOfPullData to store the data in between taking it after one source runs and copying it before another one runs. c++ doesn't seem to have a delete function for operator new that I can find, so I will use malloc here.


	GE_GlueTarget* newGlue = new GE_GlueTarget{updateData, pullData,pullOn,sizeOfPullData,bufferAlloc};
	targets.insert(targets.end(),newGlue);
	memcpy(newGlue->buffer,newGlue->pullData,newGlue->sizeOfPullData);//Copy to the buffer the pulled data - we need to do this to avoid potentially writing an unitialized value to the buffer
	pthread_mutex_unlock(&GlueMutex);

	return newGlue;
}

void GE_FreeGlueObject(GE_GlueTarget* subject)
{
	pthread_mutex_lock(&GlueMutex);
	printf("KILL GLUE OBJECT\n");
	targets.remove(subject);
	pthread_mutex_unlock(&GlueMutex); //no longer in the list of glue targets; thread-safe to unlock now

	free(subject->buffer);

	delete subject;
}
