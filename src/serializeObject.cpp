#include "serializeObject.h"

#include "GeneralEngineCPP.h"

const size_t GE_SerializeStringInitialSize = 12;




template<typename basicDataType>
void internal_serialize_basic(basicDataType serializeMe, char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	//do we have enough space?
	if ((*bufferUsed)+sizeof(basicDataType) > (*bufferSize))
	{
		(*bufferSize) += (*bufferSize)+sizeof(basicDataType); //double the array size plus the size of the data. this is the cheapest way of making sure the data will fit
		//resize the string
		(*buffer) = static_cast<char*>(realloc(*buffer,*bufferSize));
	}
	//use size_t to iterate to avoid overflowing a number, and make the computations as cheap as possible
	for (size_t i=0;i<sizeof(serializeMe);i++)
	{
		(*buffer)[static_cast<size_t>(*bufferUsed)+i] = static_cast<char>(serializeMe >> (8*((sizeof(serializeMe)-i)-1)) & 0xFF);
	}
	//add how many spaces wee used
	(*bufferUsed) += sizeof(basicDataType);
	//we used pointers, so we don't return anything cause we changed the stuff in memory.
}
template<typename basicDataType>
basicDataType internal_unserialize_basic(char* serialized,size_t* bufferUnserialized)
{
	basicDataType data = 0;
	size_t length = sizeof(basicDataType);
	for (size_t i=0;i < length;i++)
	{
		data |= static_cast<unsigned char>(serialized[i+(*bufferUnserialized)]) << (8*((length-i)-1));
	}
	(*bufferUnserialized) += length;

	return data; 
}


void GE_Serialize(GE_Serializable* serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize)//,class std::enable_if< std::is_base_of<Serializable,serializeable>::value>::type>)
{
	serializeMe->serialize(buffer,bufferUsed,bufferSize);
}

//and because C++ templates are not quite powerful enough to do this without this ugly mess, here's this... ugly mess... All the basic C++ data types
void GE_Serialize(int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize)
{
	internal_serialize_basic(serializeMe,buffer,bufferUsed,bufferSize);
}
void GE_Serialize(short int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize)
{
	internal_serialize_basic(serializeMe,buffer,bufferUsed,bufferSize);
}
void GE_Serialize(long int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize)
{
	internal_serialize_basic(serializeMe,buffer,bufferUsed,bufferSize);
}
void GE_Serialize(long long int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize)
{
	internal_serialize_basic(serializeMe,buffer,bufferUsed,bufferSize);
}
void GE_Serialize(unsigned int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize)
{
	internal_serialize_basic(serializeMe,buffer,bufferUsed,bufferSize);
}
void GE_Serialize(unsigned long int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize)
{
	internal_serialize_basic(serializeMe,buffer,bufferUsed,bufferSize);
}
void GE_Serialize(unsigned long long int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize)
{
	internal_serialize_basic(serializeMe,buffer,bufferUsed,bufferSize);
}
void GE_Serialize(unsigned short int serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize)
{
	internal_serialize_basic(serializeMe,buffer,bufferUsed,bufferSize);
}
void GE_Serialize(char serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize)
{
	internal_serialize_basic(serializeMe,buffer,bufferUsed,bufferSize);
}
void GE_Serialize(unsigned char serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize)
{
	internal_serialize_basic(serializeMe,buffer,bufferUsed,bufferSize);
}


char* GE_AllocateSerializeString()
{
	return static_cast<char*>(malloc(GE_SerializeStringInitialSize));
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


bool GE_TEST_SerializeBasic()
{
	bool passedAll = true;

	char* buffer = GE_AllocateSerializeString();
	size_t used = 0;
	size_t size = GE_SerializeStringInitialSize;


	GE_Serialize((unsigned int)0xffffffff,&buffer,&used,&size);
	GE_Serialize((unsigned int)672214,&buffer,&used,&size);
	GE_Serialize((int)-672214,&buffer,&used,&size);


	GE_FinalizeSerializeString(&buffer,&used,&size);

	size_t unserialized = 0;

	GE_TEST_STD(GE_StringifyNumber,unsigned int,0xffffffff,internal_unserialize_basic<unsigned int>,buffer,&unserialized);
	GE_TEST_STD(GE_StringifyNumber,unsigned int,672214,internal_unserialize_basic<unsigned int>,buffer,&unserialized);
	GE_TEST_STD(GE_StringifyNumber,int,-672214,internal_unserialize_basic<int>,buffer,&unserialized);


	


	GE_FreeSerializeString(buffer);


	buffer = static_cast<char*>(malloc(4));
	used = 0;
	size =4;
	GE_Serialize((unsigned int)0xffffffff,&buffer,&used,&size);

	GE_TEST_Log("If this assert failed, next assert is invalidated\n");
	GE_TEST_ASSERT(GE_StringifyNumber,used, 4, ==);
	GE_FinalizeSerializeString(&buffer,&used,&size);
	GE_TEST_Log("Ensure GE_FinalizeSerializeString can resize string\n");
	GE_TEST_ASSERT(GE_StringifyNumber,size, 4, >);



	GE_TEST_Log("Ensure GE_Serialize can resize string\n");
	buffer = static_cast<char*>(malloc(1));
	used = 0;
	size =1;
	GE_Serialize((unsigned int)0xffffffff,&buffer,&used,&size);
	GE_TEST_ASSERT(GE_StringifyNumber,size, 4, >=);
	


	return passedAll;
}
