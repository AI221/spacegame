/*!
 * @file
 * @author Jackson McNeill
 *
 * Defines a method of data serialization. 
 * The concept here is that all data types can be serialized using GE_Serialize(data,...); and unserialized with GE_Unserialize<data-type>(...);
 * This makes it much easier, as things like vectors can be serialized while keeping type safety, and only implementing their serialization method once.
 */

#pragma once

#include <type_traits>
#include <stdlib.h>

//TODO: forward declaration?
#include <vector>
#include <unordered_set>
#include <set>
#include <deque>
#include <list>

#include <map>



const size_t GE_SerializeStringInitialSize = 2048;



/*!
 * Forward declare all internal serialization functions
 * Don't try to read this, just read them at the bottom of this file where they're defined.
 */

template<typename basicDataType>
void internal_serialize_basic(basicDataType serializeMe, char** buffer, size_t* bufferUsed, size_t* bufferSize);
template<typename basicDataType>
basicDataType internal_unserialize_basic(char* serialized,size_t* bufferUnserialized);
template<typename array_t>
void internal_serialize_container_array(array_t array, size_t size,char** buffer, size_t* bufferUsed, size_t* bufferSize); 
template<typename array_t>
array_t internal_unserialize_container_array(char* serialized, size_t* bufferUnserialized, int serializedVersion);
template<typename sequence_container>
void internal_serialize_container_sequence(sequence_container container,char** buffer, size_t* bufferUsed, size_t* bufferSize);
template<typename sequence_container>
sequence_container internal_unserialize_container_sequence(char* serialized, size_t* bufferUnserialized, int serializedVersion);
template<typename map_container>
void internal_serialize_container_map(map_container container,char** buffer, size_t* bufferUsed, size_t* bufferSize);
template<typename map_container>
map_container internal_unserialize_container_map(char* serialized, size_t* bufferUnserialized, int serializedVersion);







/*!
 * If a class inherits from this, it must implement serialize and unserializae. Unserialize is static, and thus cannot be defined in the base class. Unserialize looks like this:
 *
 *	virtual static GE_Serializable* unserialize(char* serialized, size_t* bufferUnserialized, int serializationVersion)
 *
 *	...and creates a new class, of your class's type, with the 'new' operator. Doing anything else results in undefined behaviour.
 */
class GE_Serializable
{
	public:
		GE_Serializable(){};
		virtual ~GE_Serializable(){};
		virtual void serialize(char** buffer,size_t* bufferUsed, size_t* bufferSize) {}


		//consider for review:
		virtual unsigned long long int getVersion()
		{
			return 0;
		}
		virtual void appendName(char** buffer, unsigned int position)
		{

		}
};



/*!
 *
 * Helper functions
 *
 */


template<class maybe_container>
struct is_container
{
	private:
		template<class maybe_container_2> static char test_is_container(typename maybe_container_2::const_iterator*);
		template<class maybe_container_2> static int test_is_container(...);
	public:
		enum { value  = sizeof(test_is_container<typename std::remove_pointer<maybe_container>::type>(0)) == sizeof(char) };
};

/*!
 * Solution based off "David Haim"'s https://stackoverflow.com/a/35293682 
 * It might be long, but it's not as ugly as some other stuff
 */
template<class a> struct is_vectorish : std::false_type {};

template<class a> struct is_vectorish<std::vector<a>> : std::true_type{};
template<class a,class b> struct is_vectorish<std::vector<a,b>> : std::true_type{};

template<class a> struct is_vectorish<std::deque<a>> : std::true_type{};
template<class a,class b> struct is_vectorish<std::deque<a,b>> : std::true_type{};

template<class a> struct is_vectorish<std::list<a>> : std::true_type{};
template<class a,class b> struct is_vectorish<std::list<a,b>> : std::true_type{};


template<class a> struct is_vectorish<std::set<a>> : std::true_type{};
template<class a,class b> struct is_vectorish<std::set<a,b>> : std::true_type{};
template<class a,class b,class c> struct is_vectorish<std::set<a,b,c>> : std::true_type{};

template<class a> struct is_vectorish<std::unordered_set<a>> : std::true_type{};
template<class a,class b> struct is_vectorish<std::unordered_set<a,b>> : std::true_type{};
template<class a,class b,class c> struct is_vectorish<std::unordered_set<a,b,c>> : std::true_type{};
template<class a,class b,class c,class d> struct is_vectorish<std::unordered_set<a,b,c,d>> : std::true_type{};



template<class a> struct is_set : std::false_type {};

template<class a> struct is_set<std::set<a>> : std::true_type{};
template<class a,class b> struct is_set<std::set<a,b>> : std::true_type{};
template<class a,class b,class c> struct is_set<std::set<a,b,c>> : std::true_type{};


template<class a> struct is_list : std::false_type {};

template<class a> struct is_list<std::list<a>> : std::true_type{};
template<class a,class b> struct is_list<std::list<a,b>> : std::true_type{};









/*!
 * 
 * Sequence container deduction
 *
 */



//Allow vector, set, deque, unordered_set, and unordered_multiset (TODO: make it not allow unordered_multiset)
template<typename container_t>
typename std::enable_if<is_vectorish<typename std::remove_pointer<container_t>::type>::value,void>::type internal_serialize_container(container_t container,char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	internal_serialize_container_sequence(container,buffer,bufferUsed,bufferSize);
}
//Allow map
template<typename container_t>
typename std::enable_if<!(is_vectorish<typename std::remove_pointer<container_t>::type>::value),container_t>::type internal_serialize_container(container_t container,char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	internal_serialize_container_map(container,buffer,bufferUsed,bufferSize);
}






template<typename container_t>
typename std::enable_if<is_vectorish<typename std::remove_pointer<container_t>::type>::value,container_t>::type internal_unserialize_container(char* serialized, size_t* bufferUnserialized,int serializedVersion)
{
	return internal_unserialize_container_sequence<container_t>(serialized,bufferUnserialized,serializedVersion);
}
//Allow map
template<typename container_t>
typename std::enable_if<!(is_vectorish<typename std::remove_pointer<container_t>::type>::value),container_t>::type internal_unserialize_container(char* serialized, size_t* bufferUnserialized,int serializedVersion)
{
	return internal_unserialize_container_map<container_t>(serialized,bufferUnserialized,serializedVersion);
}








/*!
 *
 * GE_Serialize templates
 *
 */






template <class data>
typename std::enable_if<is_container<data>::value,void>::type GE_Serialize(data serializeMe, char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	internal_serialize_container(serializeMe,buffer,bufferUsed,bufferSize);
}
//Template for classes implementing the protocol, must be a pointer to said class.
template <class data>
typename std::enable_if<std::is_base_of<GE_Serializable,typename std::remove_pointer<data>::type>::value,void>::type GE_Serialize(data serializeMe, char** buffer, size_t* bufferUsed,size_t* bufferSize)
{
	serializeMe->serialize(buffer,bufferUsed,bufferSize);
}
//if all else fails, it must be a basic data type (or unsupported, in which case this will fail, hopefully.)
template<class data>
typename std::enable_if<std::is_arithmetic<data>::value,void>::type GE_Serialize(data serializeMe, char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	return internal_serialize_basic(serializeMe,buffer,bufferUsed,bufferSize);
}




/*!
 *
 * GE_Unserialize templates
 *
 */





template<class data>
typename std::enable_if<std::is_arithmetic<data>::value,data>::type GE_Unserialize(char* serialized, size_t* bufferUnserialized,int serializedVersion)
{
	return internal_unserialize_basic<data>(serialized,bufferUnserialized);	
}

//Template for classes implementing the protocol
//enable if the data's type--after removing its pointer--has the static function unserailize. If enabled, return the data -- which will be a pointer.
template <class data>
typename std::enable_if<std::is_function<decltype(std::remove_pointer<data>::type::unserialize)>::value,data>::type GE_Unserialize(char* serialized, size_t* bufferUnserialized,int serializedVersion)
{
	//return the result of the static unserialize function.
	return static_cast<data>(std::remove_pointer<data>::type::unserialize(serialized,bufferUnserialized,serializedVersion));
}

template <class data>
typename std::enable_if<is_container<typename std::remove_pointer<data>::type>::value,data>::type GE_Unserialize(char* serialized, size_t* bufferUnserialized,int serializedVersion)
{
	return internal_unserialize_container<data>(serialized,bufferUnserialized,serializedVersion);
}


char* GE_AllocateSerializeString(size_t* bufferUsed, size_t* bufferSize,int serialization_version);
void GE_FinalizeSerializeString(char** buffer, size_t* bufferUsed, size_t* bufferSize);;
void GE_FreeSerializeString(char* string);
int GE_GetSerializedVersion(char* string, size_t* bufferUnserialized);

bool GE_TEST_SerializeBasic();



/*!
 *
 * Serialization & unserialization base functions - do not invoke directly
 *
 */







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
	char* bytes = reinterpret_cast<char*>(&serializeMe);
	for (size_t i=0;i<sizeof(serializeMe);i++)
	{
		(*buffer)[static_cast<size_t>(*bufferUsed)+i] = bytes[(sizeof(serializeMe)-i)-1];//reinterpret_cast<char>(serializeMe >> (8*((sizeof(serializeMe)-i)-1)) & 0xFF);
	}
	//add how many spaces wee used
	(*bufferUsed) += sizeof(basicDataType);
	//we used pointers, so we don't return anything cause we changed the stuff in memory.
}
template<typename basicDataType>
basicDataType internal_unserialize_basic(char* serialized,size_t* bufferUnserialized)
{
	//TODO!!! This shouldn't memory leak, but I'd like to check it more in-depth.
	//basicDataType data = 0;
	const size_t length = sizeof(basicDataType);
	char bytes[length];
	for (size_t i=0;i < length;i++)
	{
		//data |= static_cast<unsigned char>(serialized[i+(*bufferUnserialized)]) << (8*((length-i)-1));
		bytes[((length-i)-1)] = serialized[i+(*bufferUnserialized)];
	}
	(*bufferUnserialized) += length;

	//This shouldn't memory leak (allocated on stack) 
	return *reinterpret_cast<basicDataType*>(bytes); 
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
array_t internal_unserialize_container_array(char* serialized, size_t* bufferUnserialized, int serializedVersion)
{
	//get the size
	size_t size = GE_Unserialize<size_t>(serialized,bufferUnserialized,serializedVersion);
	//allocate an array big enough to store all of this (and only that big! users must copy the stuff into their array)
	array_t* array = static_cast<array_t*>(malloc(size));
	//unserialize every element
	for (size_t i=0; i!=size;i++)
	{
		array[i] = GE_Unserialize<array_t>(serialized,bufferUnserialized,serializedVersion);
	}
	return array;	
}





template<typename sequence_container>
void internal_serialize_container_sequence(sequence_container container,char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	//serialize the length (else we would not know when to stop, during unserialization)
	GE_Serialize(container->size(),buffer,bufferUsed,bufferSize);
	for (auto &element : *container)
	{
		GE_Serialize(element,buffer,bufferUsed,bufferSize);
	}

}



template<typename sequence_container>
sequence_container internal_unserialize_container_sequence(char* serialized, size_t* bufferUnserialized, int serializedVersion)
{
	//get length
	size_t length = GE_Unserialize<size_t>(serialized,bufferUnserialized,serializedVersion);
	sequence_container container = new (typename std::remove_pointer<sequence_container>::type)();
	if constexpr (!(is_list<typename std::remove_pointer<sequence_container>::type>::value || is_set<typename std::remove_pointer<sequence_container>::type>::value))
	{
		container->reserve(length);
	}
	for (size_t i=0;i!=length;i++)
	{
		//insert into the container, the next unserialization(of the type that our container is)
		container->insert(container->end(),GE_Unserialize<typename std::remove_pointer<sequence_container>::type::value_type>(serialized,bufferUnserialized,serializedVersion));	
	}
	return container;
}





template<typename map_container>
void internal_serialize_container_map(map_container container,char** buffer, size_t* bufferUsed, size_t* bufferSize)
{
	//serialize the length (else we would not know when to stop, during unserialization)
	GE_Serialize(container->size(),buffer,bufferUsed,bufferSize);
	for (const auto &element : (*container))
	{
		GE_Serialize(element.first,buffer,bufferUsed,bufferSize);
		GE_Serialize(element.second,buffer,bufferUsed,bufferSize);
	}
}
template<typename map_container>
map_container internal_unserialize_container_map(char* serialized, size_t* bufferUnserialized, int serializedVersion)
{
	size_t length = GE_Unserialize<size_t>(serialized,bufferUnserialized,serializedVersion);
	map_container container = new (typename std::remove_pointer<map_container>::type)();
	for (size_t i=0;i!=length;i++)
	{
		//insert a pair of the key type, and mapped type. 
		
		//std::make_pair runs the code to get the SECOND key FIRST. So, go ahead and make this a one-liner for a nice mindf
		auto first = GE_Unserialize<typename std::remove_pointer<map_container>::type::key_type>(serialized,bufferUnserialized,serializedVersion);
		auto second = GE_Unserialize<typename std::remove_pointer<map_container>::type::mapped_type>(serialized,bufferUnserialized,serializedVersion);
		container->insert(container->end(),std::make_pair(first,second));
	}
	return container;
}

