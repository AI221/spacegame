#pragma once

#include <type_traits>
#include <stdlib.h>
#include <list>
#include <map>


const size_t GE_SerializeStringInitialSize = 2048;

class GE_Serializable
{
	public:
		virtual void serialize(char** buffer,size_t* bufferUsed, size_t* bufferSize) {}
		virtual void restore_serialized(char** data) {}
		
		virtual unsigned long long int getVersion()
		{
			return 0;
		}
		virtual void appendName(char** buffer, unsigned int position)
		{

		}
};



/*!
 * Internal functions, don't touch
 */
template<typename basicDataType>
void internal_serialize_basic(basicDataType serializeMe, char** buffer, size_t* bufferUsed, size_t* bufferSize);
template<typename basicDataType>
basicDataType internal_unserialize_basic(char* serialized,size_t* bufferUnserialized);

template<typename array_t>
void internal_serialize_container_array(array_t array, size_t size,char** buffer, size_t* bufferUsed, size_t* bufferSize); 
template<typename array_t>
array_t internal_unserialize_container_array(char* serialized, size_t* bufferUnserialized);

template<typename sequence_container>
void internal_serialize_container_sequence(sequence_container container,char** buffer, size_t* bufferUsed, size_t* bufferSize);
template<typename sequence_container>
sequence_container internal_unserialize_container_sequence(char* serialized, size_t* bufferUnserialized);


/*
template <class data>
typename std::enable_if<std::is_member_function_pointer<typename data::insert>::value,void>::type GE_Serialize(data serializeMe, char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	internal_unserialize_container_sequence(serializeMe,buffer,bufferUsed,bufferSize);
}
*/
template <class data>
typename std::enable_if<std::is_function<typename data::serialize>::value,void>::type GE_Serialize(data serializeMe, char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
		serializeMe->serialize(buffer,bufferUsed,bufferSize);

}


//if all else fails, it must be a basic data type (or unsupported, in which case this will fail. hopefully.)
template<class data>
void GE_Serialize(data serializeMe, char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	return internal_serialize_basic(serializeMe,buffer,bufferUsed,bufferSize);
}


template<class data>
typename std::enable_if<std::is_class<data>::value,data*>::type GE_Unserialize(char* serialized, size_t* bufferUnserialized)
{
	return data::unserialize(serialized,bufferUnserialized);	
}


template<class data>
data GE_Unserialize(char* serialized, size_t* bufferUnserialized)
{
	return internal_unserialize_basic<data>(serialized,bufferUnserialized);	
}


char* GE_AllocateSerializeString();

void GE_FreeSerializeString(char* string);



bool GE_TEST_SerializeBasic();









/*

typedef unsigned int variable_id_t;
typedef std::list<std::pair<void*,size_t>> variable_list_t;


template<class obj>
class GE_GroupSerializer
{

	public:
		GE_GroupSerializer(){}
		void registerObject(obj* object)
		{
			objectVariableSpots.insert(std::make_pair(static_cast<unsigned int>(object->ID),new variable_list_t() ));
		}

		void addValue(obj* object, void* value, size_t sizeOfValue)
		{
			/signed int lastNumber = (objectVariableSpots[object]->size()==0)? -1 : (objectVariableSpots[object]->end()-1)->first; //get the last used number. number sign allows for -1
			unsigned int myNumber = lastNumber+1; //unsigned because minimum value of last number is -1.
			for (int i=0;i<sizeOfValue;i++)
			{
				objectVariableSpots[object]->insert(myNumber);
			}/\*
			objectVariableSpots[object->ID].insert(std::make_pair(value,sizeOfValue));
		}
		std::string serialize()
		{
			std::string buffer = "";

			for (auto i : objectVariableSpots)
			{
				for (auto o : i.second)
				{
					size_t size = o.second;
					void* serializebuffer = malloc(size);
					memcpy(o.first,serializebuffer,size);
					for (int p = 0; p < size; p++)
					{
						buffer = buffer + static_cast<char>(serializebuffer >> (size*8)-(p*8) & 0xFF)
					}
					free(serializebuffer);
				}
				buffer = buffer+";";
			}
		}

		~GE_GroupSerializer()
		{
			for (auto i : objectVariableSpots)
			{
				delete i.second; //delete the std::list which is allocated with new
			}
		}

	private:
		std::map<unsigned int,variable_list_t*> objectVariableSpots;


};
*/
