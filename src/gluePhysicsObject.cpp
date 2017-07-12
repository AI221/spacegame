#include "gluePhysicsObject.h"

glueTarget targets[MAX_GLUE_TARGETS];
int countGlueTargets = -1;

void GE_glueThreadMain()
{
	pthread_mutex_lock(&PhysicsEngineMutex);
	for (int i = 0; i < coutnGlueTargets; i++)
	{
		(*(targets[i].subject)) = allPhysicsObjects[targets[i].physicsObjectID];
	}
	pthread_mutex_unlock(&PhysicsEngineMutex);
}
void GE_addSubject(Vector2r* subject, int physicsID)
{
	//TODO: is this okay to do whilst glueThreadMain is running?
	countGlueTargets++;

	targets[countGlueTargets] = glueTarget{subject, physicsID};
}

