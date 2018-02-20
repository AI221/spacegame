#include "serializeObject.h"

#include <typeinfo>
#include "GeneralEngineCPP.h"





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



template<typename array_t>
void internal_serialize_container_array(array_t array, size_t size,char** buffer, size_t* bufferUsed, size_t* bufferSize) //note, size is how many elements in array, not ammount of memory taken.
{
	//serialize the length so unserialize knows how many to read
	GE_Serialize(size,buffer,bufferUsed,bufferSize);
	for (size_t i =0;i != size;i++)
	{
		GE_Serialize(array[i],buffer,bufferUsed,bufferSize); //handles resizing for me
	}
}
template<typename array_t>
array_t internal_unserialize_container_array(char* serialized, size_t* bufferUnserialized)
{
	//get the size
	size_t size = GE_Unserialize<size_t>(serialized,bufferUnserialized);
	//allocate an array big enough to store all of this (and only that big! users must copy the stuff into their array)
	array_t* array = static_cast<array_t*>(malloc(size));
	//unserialize every element
	for (size_t i=0; i!=size;i++)
	{
		array[i] = GE_Unserialize<array_t>(serialized,bufferUnserialized);
	}
	return array;	
}

template<typename sequence_container>
void internal_serialize_container_sequence(sequence_container container,char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	//serialize the length (else we would not know when to stop, during unserialization)
	GE_Serialize(container->size(),buffer,bufferUsed,bufferSize);
	for (auto element : *container)
	{
		GE_Serialize(element,buffer,bufferUsed,bufferSize);
	}

}
template<typename sequence_container>
sequence_container internal_unserialize_container_sequence(char* serialized, size_t* bufferUnserialized)
{
	//get length
	size_t length = GE_Unserialize<size_t>(serialized,bufferUnserialized);
	sequence_container container;
	for (size_t i=0;i!=length;i++)
	{
		//insert into the container, the next unserialization(of the type that our container is)
		container.insert(container.end(),GE_Unserialize<typename sequence_container::value_type>(serialized,bufferUnserialized));	
	}
	return container;
}




void GE_Serialize(GE_Serializable* serializeMe, char** buffer,size_t* bufferUsed, size_t* bufferSize)//,class std::enable_if< std::is_base_of<Serializable,serializeable>::value>::type>)
{
	serializeMe->serialize(buffer,bufferUsed,bufferSize);
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


#include <vector>
bool GE_TEST_SerializeBasic()
{


	bool passedAll = true;

	/////////////////////////////////////////////////////////////////0
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

	/////////////////////////////////////////////////////////////////1


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

	/////////////////////////////////////////////////////////////////2


	GE_TEST_Log("Ensure GE_Serialize can resize string\n");
	buffer = static_cast<char*>(malloc(1));
	used = 0;
	size =1;
	GE_Serialize((unsigned int)0xffffffff,&buffer,&used,&size);
	GE_TEST_ASSERT(GE_StringifyNumber,size, 4, >=);
	GE_FreeSerializeString(buffer);


	/////////////////////////////////////////////////////////////////3

	buffer = GE_AllocateSerializeString();
	used = 0;
	size = GE_SerializeStringInitialSize;

	std::vector<int> myvec;
	myvec.insert(myvec.end(),27);
	myvec.insert(myvec.end(),26);

	internal_serialize_container_sequence(&myvec,&buffer,&used,&size);

	unserialized = 0;

	std::vector<int> unservec = internal_unserialize_container_sequence<std::vector<int>>(buffer,&unserialized);

	GE_TEST_Log("Ensure vector unserialized properly\n");
	GE_TEST_ASSERT(GE_StringifyNumber,*unservec.begin(),27,==);
	GE_TEST_Log("Ditto\n");
	GE_TEST_ASSERT(GE_StringifyNumber,*(unservec.begin()+1),26,==);

	GE_FreeSerializeString(buffer);
	


	return passedAll;
}
