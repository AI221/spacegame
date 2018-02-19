#pragma once

#include <stdlib.h>
#include <list>
#include <map>

extern const size_t bufferInitialSize;


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






void GE_Serialize(GE_Serializable* serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize);//,class std::enable_if< std::is_base_of<Serializable,serializeable>::value>::type>)

void GE_Serialize(int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize);

void GE_Serialize(short int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize);

void GE_Serialize(long int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize);

void GE_Serialize(long long int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize);

void GE_Serialize(unsigned int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize);

void GE_Serialize(unsigned long int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize);

void GE_Serialize(unsigned long long int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize);

void GE_Serialize(unsigned short int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize);

void GE_Serialize(char serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize);

void GE_Serialize(unsigned char serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize);

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
