#pragma once

#include <stdlib.h>
#include <list>
#include <map>

typedef unsigned int variable_id_t;
typedef std::list<std::pair<void*,size_t>> variable_list_t;


/*
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
			/*signed int lastNumber = (objectVariableSpots[object]->size()==0)? -1 : (objectVariableSpots[object]->end()-1)->first; //get the last used number. number sign allows for -1
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
