#include "serializeObject.h"

#include <set>
#include <map>
#include <stack>
#include "serialize.h"

//TODO add table with functions for serialization

#define padding_bytes 0xf0f0f0f0f0f0f0f0
#define PADDING_CHECK

typedef std::map<unsigned int, unserializationFunctionDef_t> unserializeFunctions_t;

unserializeFunctions_t unserializeFunctions;



struct noUnserializationFunction : std::exception
{
	noUnserializationFunction(unsigned int type)
	{
		this->type = type;
	}
	unsigned int type;
	const char* what() const noexcept { return ("Tried to unserialize type "+std::to_string(type)+" which does not have an unserialize function registered.").c_str(); }
};
class GE_TrackedObject : GE_Serializable
{
	public:
		GE_TrackedObject(unsigned int type, GE_PhysicsObject* obj)
		{
			this->type = type;
			this->obj = obj;
		}
		~GE_TrackedObject(){} //do nothing, as the physics object is managed by the physics engine

		unsigned int type;
		GE_PhysicsObject* obj;
		void serialize(char** buffer,size_t* bufferUsed, size_t* bufferSize)
		{
			GE_Serialize(static_cast<unsigned long long>(padding_bytes),buffer,bufferUsed,bufferSize); //useful for data recovery & diagnostics in a misalignment error.
			GE_Serialize(type,buffer,bufferUsed,bufferSize);
			GE_Serialize(obj,buffer,bufferUsed,bufferSize);
		}
		static GE_TrackedObject* unserialize(char* buffer, size_t* bufferUnserialized,int version)
		{
			unsigned long long padding = GE_Unserialize<unsigned long long>(buffer,bufferUnserialized,version);
#ifdef PADDING_CHECK
			if (padding != padding_bytes)
			{
				printf("WARNING!! You have a misalignment -- padding was not 0xf0f0...\n");
			}
#endif
			//read back type
			unsigned int type = GE_Unserialize<unsigned int>(buffer,bufferUnserialized,version);
			//call unserialization function for type
			
			auto it = unserializeFunctions.find(type);
			if (it == unserializeFunctions.end())
			{
				throw new noUnserializationFunction(type);
				return NULL; //so that the compiler will take a hint and not give me a warning
			}
			
			GE_PhysicsObject* newObject = it->second(buffer,bufferUnserialized,version);

			return new GE_TrackedObject(type,newObject);
		}
};

typedef std::list<GE_TrackedObject*> trackedObjects_t;

trackedObjects_t* trackedObjects;
std::stack<GE_TrackedObject*> trackedObjectsBuffer;
int currentVersion;

void physicsCallback()
{
	while (!trackedObjectsBuffer.empty())
	{
		trackedObjects->insert(trackedObjects->end(),trackedObjectsBuffer.top());
		trackedObjectsBuffer.pop();
	}	
}

int GE_Init_SerializeObject() //TODO: Shutdown function
{
	GE_AddPhysicsDoneCallback(physicsCallback);
	trackedObjects = new trackedObjects_t();
	GE_SetGameSerializationVersion(1); //TODO temp
	printf("Initialized serialize object\n");
	return 0;
}

void GE_SetGameSerializationVersion(int version)
{
	currentVersion = version;
}

struct noSerializationFunction : std::exception
{
	noSerializationFunction(unsigned int type)
	{
		this->type = type;
	}
	unsigned int type;
	const char* what() const noexcept { return ("Tried to add tracked object of type "+std::to_string(type)+" which does not have an unserialize function registered.").c_str(); }
};

/*!
 * Okay to call any time (on the physics thread), but you must wait until the end of the physics tick to remove yourself from the buffer or else it won't work.
 */
GE_TrackedObject* GE_AddTrackedObject(unsigned int type, GE_PhysicsObject* obj)
{
	if (!unserializeFunctions[type])
	{
		throw new noSerializationFunction(type);
	}
		
	auto newInsert = new GE_TrackedObject{type,obj};
	trackedObjectsBuffer.push(newInsert);
	return newInsert;
}

/*!
 * Do NOT call when you're in a serialization function
 *
 * For example, it is not okay to call this while being serialized, unserialized, or while in your constructor which was called from an unserialization.
 */
void GE_RemoveTrackedObject(GE_TrackedObject* object)
{
	trackedObjects->remove(object);
}
void GE_RegisterUnserializationFunction(unsigned int type, unserializationFunctionDef_t function)
{
	unserializeFunctions.insert(std::make_pair(type,function));	
}

char* GE_SerializedTrackedObjects(size_t* bufferUsed, size_t* bufferSize)
{
	char* buffer = GE_AllocateSerializeString(bufferUsed,bufferSize,currentVersion);

	GE_Serialize(trackedObjects,&buffer,bufferUsed,bufferSize); 

	return buffer;
}

#include <SDL2/SDL.h>
void GE_UnserializeTrackedObjects(char* buffer)
{
	size_t bufferUnserialized;
	int version = GE_GetSerializedVersion(buffer,&bufferUnserialized);

	delete trackedObjects; //remove any existing objects

		trackedObjects = GE_Unserialize<trackedObjects_t*>(buffer,&bufferUnserialized,version);
	try
	{
	}
	catch (std::exception& bad_function_call)
	{
		printf("Failed to unserialize data -- Corrupt!\n");
		SDL_Delay(10000);
	}
}	


