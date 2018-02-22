#include "serializeObject.h"

#include <set>
#include <map>
#include "serialize.h"

//TODO add table with functions for serialization

#define padding_bytes 0xf0f0f0f0f0f0f0f0

typedef std::map<unsigned int, unserializationFunctionDef_t> unserializeFunctions_t;

unserializeFunctions_t unserializeFunctions;

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
			return new GE_TrackedObject(type,unserializeFunctions[type](buffer,bufferUnserialized,version));
		}
};

typedef std::list<GE_TrackedObject*> trackedObjects_t;

trackedObjects_t* trackedObjects;
int currentVersion;


int GE_Init_SerializeObject()
{
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

GE_TrackedObject* GE_AddTrackedObject(unsigned int type, GE_PhysicsObject* obj)
{
	if (!unserializeFunctions[type])
	{
		throw new noSerializationFunction(type);
	}
		
	auto newInsert = new GE_TrackedObject{type,obj};
	trackedObjects->insert(trackedObjects->begin(),newInsert);
	return newInsert;
}
void GE_RemoveTrackedObject(GE_TrackedObject* object)
{
	trackedObjects->remove(object);
}
void GE_RegisterUnserializationFunction(unsigned int type, unserializationFunctionDef_t function)
{
	unserializeFunctions.insert(std::make_pair(type,function));	
}

char* GE_SerializedTrackedObjects()
{
	size_t bufferUsed;
	size_t bufferSize;
	char* buffer = GE_AllocateSerializeString(&bufferUsed,&bufferSize,currentVersion);

	printf("tracked %d\n",trackedObjects);

	GE_Serialize(trackedObjects,&buffer,&bufferUsed,&bufferSize); 

	return buffer;
}

void GE_UnserializeTrackedObjects(char* buffer)
{
	size_t bufferUnserialized;
	int version = GE_GetSerializedVersion(buffer,&bufferUnserialized);

	delete trackedObjects; //remove any existing objects

	trackedObjects = GE_Unserialize<trackedObjects_t*>(buffer,&bufferUnserialized,version);
}	


