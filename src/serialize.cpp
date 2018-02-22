#include "serialize.h"

#include <typeinfo>
#include "GeneralEngineCPP.h"








char* GE_AllocateSerializeString(size_t* bufferUsed, size_t* bufferSize,int serialization_version)
{
	*bufferUsed = 0;
	*bufferSize = GE_SerializeStringInitialSize;
	char* buffer = static_cast<char*>(malloc(*bufferSize));

	GE_Serialize(serialization_version,&buffer,bufferUsed,bufferSize);
	return buffer;
}

void GE_FreeSerializeString(char* string)
{
	free(string);
}
void GE_FinalizeSerializeString(char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	if (*bufferUsed == *bufferSize) //if there's no room left (Because we just want ONE character!!)
	{
		(*bufferSize) += 1;
		(*buffer) = static_cast<char*>(realloc(*buffer,*bufferSize));
	}
	(*buffer)[*bufferUsed+1] = '\0';
}

int GE_GetSerializedVersion(char* string, size_t* bufferUnserialized)
{
	*bufferUnserialized =0;
	return internal_unserialize_basic<int>(string,bufferUnserialized);
}


//Tests:

#include <vector>
#include <set>
#include <map>

class serialization_test : public GE_Serializable
{
	public:
		serialization_test(int number,int number2)
		{
			myintvector.push_back(number);
			myintvector.push_back(number2);
		}
		void serialize(char** buffer,size_t* bufferUsed, size_t* bufferSize)
		{
			GE_Serialize(&myintvector,buffer,bufferUsed,bufferSize);
		}
		static GE_Serializable* unserialize(char* serialized, size_t* bufferUnserialized,int serializationVersion) 
		{
			std::vector<int>* t = GE_Unserialize<std::vector<int>*>(serialized,bufferUnserialized,serializationVersion);
			return new serialization_test(*t->begin(),*(t->begin()+1));
		}
		int returnNumber1 ()
		{
			return *myintvector.begin();
		}
		int returnNumber2 ()
		{
			return *(myintvector.begin()+1);
		}
	private:
		std::vector<int> myintvector;
};

bool GE_TEST_SerializeBasic()
{


	bool passedAll = true; //unit testing will automatically set this

	size_t used;
	size_t size;

	/////////////////////////////////////////////////////////////////0 -- Testing basic datatype serialization
	char* buffer = GE_AllocateSerializeString(&used,&size,0);


	GE_Serialize((unsigned int)0xffffffff,&buffer,&used,&size);
	GE_Serialize((unsigned int)672214,&buffer,&used,&size);
	GE_Serialize((int)-672214,&buffer,&used,&size);
	GE_Serialize((double)64563.33f,&buffer,&used,&size);


	GE_FinalizeSerializeString(&buffer,&used,&size);

	size_t unserialized;
	int v = GE_GetSerializedVersion(buffer,&unserialized);


	GE_TEST_STD(GE_StringifyNumber,unsigned int,0xffffffff,GE_Unserialize<unsigned int>,buffer,&unserialized,v);
	GE_TEST_STD(GE_StringifyNumber,unsigned int,672214,GE_Unserialize<unsigned int>,buffer,&unserialized,v);
	GE_TEST_STD(GE_StringifyNumber,int,-672214,GE_Unserialize<int>,buffer,&unserialized,v);
	GE_TEST_STD(GE_StringifyNumber,double,64563.33f,GE_Unserialize<double>,buffer,&unserialized,v);
	GE_FreeSerializeString(buffer);

	/////////////////////////////////////////////////////////////////1 -- testing string resizing on GE_FinalizeSerializeString


	buffer = static_cast<char*>(malloc(4));
	used = 0;
	size =4;
	GE_Serialize((unsigned int)0xffffffff,&buffer,&used,&size);

	GE_TEST_Log("If this assert failed, next assert is invalidated\n");
	GE_TEST_ASSERT(GE_StringifyNumber,used, 4, ==);
	GE_FinalizeSerializeString(&buffer,&used,&size);
	GE_TEST_Log("Ensure GE_FinalizeSerializeString can resize string\n");
	GE_TEST_ASSERT(GE_StringifyNumber,size, 4, >);
	GE_FreeSerializeString(buffer);

	/////////////////////////////////////////////////////////////////2 -- testing string resizing on GE_Serialize


	GE_TEST_Log("Ensure GE_Serialize (using internal_serialize_basic) can resize string\n");
	buffer = static_cast<char*>(malloc(1));
	used = 0;
	size =1;
	GE_Serialize((unsigned int)0xffffffff,&buffer,&used,&size);
	GE_TEST_ASSERT(GE_StringifyNumber,size, 4, >=);
	GE_FreeSerializeString(buffer);


	/////////////////////////////////////////////////////////////////3  -- testing vector

	buffer = GE_AllocateSerializeString(&used,&size,0);

	std::vector<int> preserialized_vector;
	preserialized_vector.insert(preserialized_vector.end(),27);
	preserialized_vector.insert(preserialized_vector.end(),26);

	GE_Serialize(&preserialized_vector,&buffer,&used,&size);

	unserialized = 99999;
	v= GE_GetSerializedVersion(buffer,&unserialized);

	std::vector<int>* unserialized_vector = GE_Unserialize<std::vector<int>*>(buffer,&unserialized,v);

	GE_TEST_Log("Ensure vector unserialized properly\n");
	GE_TEST_ASSERT(GE_StringifyNumber,*unserialized_vector->begin(),*preserialized_vector.begin(),==);
	GE_TEST_Log("Ditto\n");
	GE_TEST_ASSERT(GE_StringifyNumber,*(unserialized_vector->begin()+1),*(preserialized_vector.begin()+1),==);

	delete unserialized_vector;

	GE_FreeSerializeString(buffer);
	
	/////////////////////////////////////////////////////////////////4 -- testing classes
	
	buffer = GE_AllocateSerializeString(&used,&size,994645232);


	auto myclass = new serialization_test(1337,88);


	GE_Serialize(myclass,&buffer,&used,&size);

	delete myclass;
	unserialized = 0;
	v = GE_GetSerializedVersion(buffer,&unserialized);

	GE_TEST_Log("Ensure serialization versioning works properly\n");
	GE_TEST_ASSERT(GE_StringifyNumber,v,994645232,==);

	serialization_test* unserializedclass = (GE_Unserialize<serialization_test*>(buffer,&unserialized,v));

	GE_TEST_Log("Ensure unserialization worked\n");
	GE_TEST_ASSERT(GE_StringifyNumber,unserializedclass->returnNumber1(),1337,==);
	GE_TEST_ASSERT(GE_StringifyNumber,unserializedclass->returnNumber2(),88,==);

	delete unserializedclass;

	GE_FreeSerializeString(buffer);

	/////////////////////////////////////////////////////////////////5 -- A vector of a class w/ a vector of ints
	
	buffer = GE_AllocateSerializeString(&used,&size,0);

	std::vector<serialization_test*>* classvec = new std::vector<serialization_test*>;
	classvec->push_back(new serialization_test(1,2));
	classvec->push_back(new serialization_test(3,4));

	GE_Serialize(classvec,&buffer,&used,&size);	

	v = GE_GetSerializedVersion(buffer,&unserialized); //sets unserialized to 0 for me

	std::vector<serialization_test*>* unser_classvec = GE_Unserialize<std::vector<serialization_test*>*>(buffer,&unserialized,v);

	GE_TEST_Log("Ensure unserialization of vector (of serializable class worked)\n");
	GE_TEST_ASSERT(GE_StringifyNumber,(*unser_classvec->begin())->returnNumber1(),1,==);
	GE_TEST_ASSERT(GE_StringifyNumber,(*unser_classvec->begin())->returnNumber2(),2,==);
	GE_TEST_ASSERT(GE_StringifyNumber,(*(unser_classvec->begin()+1))->returnNumber1(),3,==);
	GE_TEST_ASSERT(GE_StringifyNumber,(*(unser_classvec->begin()+1))->returnNumber2(),4,==);


	delete classvec;

	GE_FreeSerializeString(buffer);

	/////////////////////////////////////////////////////////////////6 -- testing map containers

	buffer = GE_AllocateSerializeString(&used,&size,0);

	std::map<int,int>* maptest = new std::map<int,int>;

	maptest->insert(std::make_pair(10,20));
	maptest->insert(std::make_pair(30,40));

	GE_Serialize(maptest,&buffer,&used,&size);

	delete maptest;

	v = GE_GetSerializedVersion(buffer,&unserialized); //sets unserialized to 0 for me

	auto unser_map = GE_Unserialize<std::map<int,int>*>(buffer,&unserialized,v);

	GE_TEST_ASSERT(GE_StringifyNumber,(*maptest->begin()).first,10,==);
	GE_TEST_Log("Ensure unserialization of map of ints worked (via iteration through the map)\n");
	int i = 0;
	for (const auto &element : (*unser_map))
	{
		i=i+10;
		GE_TEST_ASSERT(GE_StringifyNumber,element.first,i,==);
		i=i+10;
		GE_TEST_ASSERT(GE_StringifyNumber,element.second,i,==);
	}

	delete unser_map;
	GE_FreeSerializeString(buffer);

	
	/////////////////////////////////////////////////////////////////7 -- testing empty vector
	buffer = GE_AllocateSerializeString(&used,&size,0);

	std::vector<int> preserialized_empty_vector;
	GE_Serialize(&preserialized_empty_vector,&buffer,&used,&size);

	unserialized = 99999;
	v = GE_GetSerializedVersion(buffer,&unserialized);

	std::vector<int>* unserialized_empty_vector = GE_Unserialize<std::vector<int>*>(buffer,&unserialized,v);

	GE_TEST_Log("Ensure empty vector unserialized properly\n");
	GE_TEST_ASSERT(GE_StringifyNumber,unserialized_empty_vector->size(),0,==);
	delete unserialized_empty_vector;

	/////////////////////////////////////////////////////////////////8 -- testing list containers

	buffer = GE_AllocateSerializeString(&used,&size,0);

	std::list<int> preserialized_list;
	preserialized_list.insert(preserialized_list.end(),27);
	preserialized_list.insert(preserialized_list.end(),26);

	GE_Serialize(&preserialized_list,&buffer,&used,&size);

	unserialized = 99999;
	v= GE_GetSerializedVersion(buffer,&unserialized);

	std::list<int>* unserialized_list = GE_Unserialize<std::list<int>*>(buffer,&unserialized,v);

	GE_TEST_Log("Ensure list unserialized properly\n");
	GE_TEST_ASSERT(GE_StringifyNumber,*unserialized_list->begin(),*preserialized_list.begin(),==);
	GE_TEST_Log("Ditto\n");
	//GE_TEST_ASSERT(GE_StringifyNumber,*(unserialized_list->begin()+1),*(preserialized_list.begin()+1),==);

	delete unserialized_list;

	GE_FreeSerializeString(buffer);

	/////////////////////////////////////////////////////////////////9 -- testing set containers
	

	buffer = GE_AllocateSerializeString(&used,&size,0);
	
	std::set<int>* settest = new std::set<int>;

	settest->insert(1);
	settest->insert(2);

	GE_Serialize(settest,&buffer,&used,&size);

	v = GE_GetSerializedVersion(buffer,&unserialized); //sets unserialized to 0 for me

	auto unser_set = GE_Unserialize<std::set<int>*>(buffer,&unserialized,v);

	GE_TEST_Log("Ensure sets work\n");
	GE_TEST_ASSERT(GE_StringifyNumber,*(unser_set->begin()),1,==);

	i=1;
	for (const auto &element : (*unser_set))
	{
		GE_TEST_ASSERT(GE_StringifyNumber,element,i,==);
		i++;
	}
	delete settest;

	/////////////////////////////////////////////////////////////////10 - ?


	return passedAll;
}
