#include <pthread.h>

#include "stdio.h"
#include "physics.h"
#include "vector2.h"
#include "renderedObject.h"

//LIMITS

#define MAX_GLUE_TARGETS 1024

#ifndef __GLUE_PHYSICS_OBJECT_INCLUDED
#define __GLUE_PHYSICS_OBJECT_INCLUDED

struct glueTarget
{
	Vector2r* subject;
	int physicsObjectID; 
};

extern glueTarget targets[MAX_GLUE_TARGETS];
extern int countGlueTargets;

void* GE_glueThreadMain(void* );
void GE_addGlueSubject(Vector2r* subject, int physicsID);

#endif //__GLUE_PHYSICS_OBJECT_INCLUDED
