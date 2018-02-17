#include "gluePhysicsObjectInit.h"

#include "gluePhysicsObject.h"
#include "physics.h"
int GE_GlueInit()
{
	GE_AddPhysicsPreCallback(GE_GluePreCallback);
	GE_AddPhysicsDoneCallback(GE_GlueCallback);

	return 0;
}
