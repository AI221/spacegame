/*!
 * @file
 * @author Jackson McNeill
 * Allows for easy serialization of physics objects with multiple class types
 */

#include "physics.h"

#include <functional>
#include <exception>

class GE_Networkable
{
	public:
		/*!
		 * Updates values from a partial serialization. 
		 *
		 * Note, you must read everything written by your partial serialization. Failure to do so will misalign everything after you, and almost certainly crash.
		 */
		virtual void GE_Update(char* buffer, size_t* bufferUnserialized) = 0;
		/*!
		 * Serializes the values that have changed and should be synced.
		 *
		 * Note, you must read everything you wrote in the GE_Update function or you will misalign everything.
		 */
		virtual void GE_PartialSerialization(char** buffer,size_t* bufferUsed, size_t* bufferSize) = 0;

};

struct GE_TrackedObject;
typedef std::function<GE_PhysicsObject*(char* buffer, size_t* bufferUnserialized, int version)> unserializationFunctionDef_t;

int GE_Init_SerializeObject();


struct noSerializationFunction;
struct noUnserializationFunction;

/*!
 * Adds an object to be 'tracked' (serialized, unserialized, state synchronization) by the serializeObject system.
 *
 * Throws noSerializationFunction exception if you forget to call !!! for the type of the object you add. You are not expected to handle this exception; rather, your program should crash. 
 * The exception should never throw in production, because object types should be assigned right after engine startup.
 */
GE_TrackedObject* GE_AddTrackedObject(unsigned int type, GE_PhysicsObject* obj);
void GE_RemoveTrackedObject(GE_TrackedObject* object);

void GE_SetGameSerializationVersion(int version);

void GE_RegisterUnserializationFunction(unsigned int type, unserializationFunctionDef_t function);

char* GE_SerializedTrackedObjects(size_t* bufferUsed, size_t* bufferSize);
void GE_UnserializeTrackedObjects(char* buffer);
