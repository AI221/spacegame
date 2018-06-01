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

/*!
 * Adds an object to be 'tracked' (serialized, unserialized, state synchronization) by the serializeObject system.
 *
 * Throws noSerializationFunction exception if you forget to call GE_RegisterUnserializationFunction for the type of the object you add. You are not expected to handle this exception; rather, your program should crash. 
 * The exception should never throw in production, because object types should be assigned right after engine startup.
 */
class GE_TrackMeAsSerializable
{
	public:
		/*!
		 * Okay to call any time (on the physics thread), but you must wait until the end of the physics tick to remove yourself from the buffer or else it won't work.
		 */
		GE_TrackMeAsSerializable(unsigned int type, GE_PhysicsObject* object);

		/*!
		 * Do NOT call when you're in a serialization function
		 *
		 * For example, it is not okay to call this while being serialized, unserialized, or while in your constructor which was called from an unserialization.
		 */
		~GE_TrackMeAsSerializable();

		GE_TrackMeAsSerializable(const GE_TrackMeAsSerializable&) = delete;
	private:
		GE_TrackedObject* tracked_object;
	


};


typedef std::function<GE_PhysicsObject*(serialization::unserialization_state&)> unserializationFunctionDef_t;

int GE_Init_SerializeObject();


struct noSerializationFunction;
struct noUnserializationFunction;


void GE_SetGameSerializationVersion(int version);

void GE_RegisterUnserializationFunction(unsigned int type, unserializationFunctionDef_t function);

void GE_SerializedTrackedObjects(serialization::serialization_state& state);
void GE_UnserializeTrackedObjects(serialization::unserialization_state& state);
