/*!
 * @file
 * @author Jackson McNeill
 *
 * Defines a method of data serialization. 
 * The concept here is that all data types can be serialized using serialize(data,state) and unserialized with unserialize<data-type>(state);
 * This makes it much easier, as things like vectors can be serialized while keeping type safety, and only implementing their serialization method once.
 */

#pragma once

#include <type_traits>
#include <stdlib.h>
#include <cassert>

//TODO: forward declaration?
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <deque>
#include <list>
#include <map>

#include "GeneralEngineCPP.h"



/*!
 * The serialization module is a static class which provides constexpr serialization functions. If the type does not inherit from ::polymorphic_serialization then 
 * the serialization function will be selected at compile time. Otherwise, the virtual function serialize will be called. For classes which do not inherit from ::polymorphic_serialization, serialize should be a constexpr function.
 *
 * Unserialization must always know the type at compile time. The serialize_object_module provides object serialization which is polymorphic
 */
namespace serialization
{




	/*!
	 * Portable size_t
	 * You MUST use this when serializing a size_t
	 */
	typedef unsigned long long size_tp;



	/*!
	 * Serialization strings will be initially this size. 
	 * When a serialization runs out of space, it doubles the size plus the size of the current basic type being serialized.
	 */
	static constexpr const size_t initial_string_size = 2048;


	static_assert(initial_string_size > sizeof(int), "Inital serialization string size must be greater than sizeof(int)"); //Just to ensure correctness




	/*!
	 * TODO: String getter functions
	 */
	struct serialization_state
	{
		GE_FORCE_INLINE serialization_state(int serialization_version);
		GE_FORCE_INLINE ~serialization_state()
		{
			free(buffer);
		}
		char* buffer;
		size_t bufferUsed;
		size_t bufferSize;

		serialization_state(const serialization_state&) = delete;
	};
	struct unserialization_state
	{
		GE_FORCE_INLINE unserialization_state(char* string);
		char* serialized;
		size_t bufferUnserialized;
		int serializedVersion;

		unserialization_state(const unserialization_state&) = delete;
	};






	template<typename associative_container>
	associative_container unserialize_container_associative(unserialization_state& state);
	template<typename associative_container>
	constexpr void serialize_container_associative(associative_container container,serialization_state& state);
	template<typename sequence_container>
	sequence_container unserialize_container_sequence(unserialization_state& state);
	template<typename sequence_container>
	constexpr void serialize_container_sequence(sequence_container container,serialization_state& state);
	template<typename array_t>
	array_t unserialize_container_array(unserialization_state& state);
	template<typename array_t>
	constexpr void serialize_container_array(array_t array, serialization_state& state);
	template<typename basicDataType>
	basicDataType unserialize_basic(unserialization_state& state);
	template<typename basicDataType>
	void serialize_basic(basicDataType serializeMe, serialization_state& state);


	serialization_state::serialization_state(int serialization_version)
	{
		bufferUsed = 0;
		bufferSize = initial_string_size;
		buffer = static_cast<char*>(malloc(bufferSize));

		serialize_basic(serialization_version,*this);

	}

	unserialization_state::unserialization_state(char* string)
	{
		serialized = string;
		bufferUnserialized =0;
		serializedVersion = -1; //unserialize_basic is not supposed to use this value, but initialize it to -1 just in case.
		serializedVersion = unserialize_basic<int>(*this);
	}



	enum container_classification
	{
		not_a_container,
		sequence,
		associative,
	};

	/*!
	 * Solution based off "David Haim"'s https://stackoverflow.com/a/35293682 
	 * It might be long, but it's not as ugly as some other stuff
	 */

	//Sequence containers
	template<class a> struct is_vector : std::false_type {};

	template<class a> struct is_vector<std::vector<a>> : std::true_type {};
	template<class a,class b> struct is_vector<std::vector<a,b>> : std::true_type {};

	template<class a> struct is_deque : std::false_type {};

	template<class a> struct is_deque<std::deque<a>> : std::true_type {};
	template<class a,class b> struct is_deque<std::deque<a,b>> : std::true_type {};

	template<class a> struct is_list : std::false_type {};

	template<class a> struct is_list<std::list<a>> : std::true_type {};
	template<class a,class b> struct is_list<std::list<a,b>> : std::true_type {};

	template<class a> struct is_set : std::false_type {};

	template<class a> struct is_set<std::set<a>> : std::true_type {};
	template<class a,class b> struct is_set<std::set<a,b>> : std::true_type {};
	template<class a,class b,class c> struct is_set<std::set<a,b,c>> : std::true_type {};

	template<class a> struct is_unordered_set : std::false_type {};

	template<class a> struct is_unordered_set<std::unordered_set<a>> : std::true_type {};
	template<class a,class b> struct is_unordered_set<std::unordered_set<a,b>> : std::true_type {};
	template<class a,class b,class c> struct is_unordered_set<std::unordered_set<a,b,c>> : std::true_type {};
	template<class a,class b,class c,class d> struct is_unordered_set<std::unordered_set<a,b,c,d>> : std::true_type {};

	//Associative containers
	template<class a> struct is_map : std::false_type {};

	template<class a,class b> struct is_map<std::map<a,b>> : std::true_type {};
	template<class a,class b,class c> struct is_map<std::map<a,b,c>> : std::true_type {};
	template<class a,class b,class c,class d> struct is_map<std::map<a,b,c,d>> : std::true_type {};

	template<class a> struct is_unordered_map : std::false_type {};

	template<class a,class b> struct is_unordered_map<std::unordered_map<a,b>> : std::true_type {};
	template<class a,class b,class c> struct is_unordered_map<std::unordered_map<a,b,c>> : std::true_type {};
	template<class a,class b,class c,class d> struct is_unordered_map<std::unordered_map<a,b,c,d>> : std::true_type {};


	template<typename maybe_container>
	static constexpr container_classification get_container_classification()
	{
		if constexpr 
			(
				is_unordered_set<maybe_container>::value ||
				is_vector<maybe_container>::value ||
				is_deque<maybe_container>::value ||
				is_list<maybe_container>::value ||
				is_set<maybe_container>::value 
			)
		{
			return sequence;
		}
		else if constexpr 
			(
				is_unordered_map<maybe_container>::value ||
			 	is_map<maybe_container>::value 
			)
		{
			return associative;
		}
		return not_a_container;
	}

	//tests
	static_assert(get_container_classification<std::vector<int>>() == sequence,"get_container_classification recognizes vector");



	/*!
	 * If a class inherits from this, it must implement serialize and unserializae. Unserialize is static, and thus cannot be defined in the base class. Unserialize looks like this:
	 *
	 *	virtual static polymorphic_serialization* unserialize(char* serialized, size_t* bufferUnserialized, int serializationVersion)
	 *
	 *	...and creates a new class, of your class's type, with the 'new' operator. Doing anything else results in undefined behaviour.
	 */
	class polymorphic_serialization
	{
		public:
			virtual void serialize(serialization_state& state) = 0;
			/*
			polymorphic_serialization(){};
			virtual ~polymorphic_serialization(){};
			virtual void serialize(char** buffer,size_t* bufferUsed, size_t* bufferSize) {}


			//consider for review:
			virtual unsigned long long int getVersion()
			{
				return 0;
			}
			virtual void appendName(char** buffer, unsigned int position)
			{

			}
			*/
	};



	/*!
	 *
	 * serialize templates
	 *
	 */

	//Container specialization
	template <class data>
	constexpr typename std::enable_if<get_container_classification<typename std::remove_pointer<data>::type>() != not_a_container,void>::type serialize(data serializeMe,serialization_state& state)
	{
		constexpr auto classification = get_container_classification<typename std::remove_pointer<data>::type>();
		if constexpr(classification == sequence)
		{
			serialize_container_sequence(serializeMe,state);
		}
		else 
		{
			serialize_container_associative(serializeMe,state);
		}
	}
	//Polymorphic specialization. Prioritized before constexpr specialization. Intentionally non-constexpr
	template <class data>
	typename std::enable_if<std::is_base_of<polymorphic_serialization,typename std::remove_pointer<data>::type>::value,void>::type 
	serialize(data serializeMe, serialization_state& state)
	{
		serializeMe->serialize(state);
	}

	//Constexpr class specialization. Used on classes which do opt into polymorphic compatible & non-constexpr serialization
	template <class data>
	constexpr typename std::enable_if<std::is_function<decltype(std::remove_pointer<data>::type::serialize)>::value && !std::is_base_of<polymorphic_serialization,typename std::remove_pointer<data>::type>::value,void>::type
	serialize(data serializeMe, serialization_state& state)
	{
		serializeMe->serialize(serializeMe,state);
	}

	//Basic datatype specialization
	template<class data>
	constexpr typename std::enable_if<std::is_arithmetic<data>::value,void>::type serialize(data serializeMe, serialization_state& state)
	{
		return serialize_basic(serializeMe,state);
	}




	/*!
	 *
	 * unserialize templates
	 *
	 */


	template<class data>
	constexpr typename std::enable_if<std::is_arithmetic<data>::value,data>::type unserialize(unserialization_state& state)
	{
		return unserialize_basic<data>(state);
	}

	//Template for classes implementing the protocol
	//enable if the data's type--after removing its pointer--has the static function unserailize. If enabled, return the data -- which will be a pointer.
	template <class data>
	constexpr typename std::enable_if<std::is_function<decltype(std::remove_pointer<data>::type::unserialize)>::value,data>::type unserialize(unserialization_state& state)
	{
		//return the result of the static unserialize function.
		return static_cast<data>(std::remove_pointer<data>::type::unserialize(state));
	}

	template <class data>
	constexpr typename std::enable_if<get_container_classification<typename std::remove_pointer<data>::type>() != not_a_container,data>::type unserialize(unserialization_state& state)
	{
		constexpr auto classification = get_container_classification<typename std::remove_pointer<data>::type>();
		if constexpr(classification == sequence)
		{
			return unserialize_container_sequence<data>(state);
		}
		else 
		{
			return unserialize_container_associative<data>(state);
		}
	}


	GE_TEST_TYPE_RETURN unit_test();




	/*!
	 *
	 * Serialization & unserialization base functions - do not invoke directly
	 *
	 */







	template<typename basicDataType>
	void serialize_basic(basicDataType serializeMe, serialization_state& state)
	{
		//do we have enough space?
		if ((state.bufferUsed)+sizeof(basicDataType) > (state.bufferSize))
		{
			state.bufferSize += (state.bufferSize)+sizeof(basicDataType); //double the array size plus the size of the data. this is the computationally cheapest way of making sure the data will fit
			//resize the string
			state.buffer = static_cast<char*>(  realloc(state.buffer,state.bufferSize)  );
		}
		//use size_t to iterate to avoid overflowing a number, and make the computations as cheap as possible
		char* bytes = reinterpret_cast<char*>(&serializeMe);
		for (size_t i=0;i<sizeof(serializeMe);i++)
		{
			(state.buffer)[static_cast<size_t>(state.bufferUsed)+i] = bytes[(sizeof(serializeMe)-i)-1];//reinterpret_cast<char>(serializeMe >> (8*((sizeof(serializeMe)-i)-1)) & 0xFF);
		}
		//add how many spaces wee used
		state.bufferUsed += sizeof(basicDataType);
	}


	template<typename basicDataType>
	basicDataType unserialize_basic(unserialization_state& state)
	{
		//TODO!!! This shouldn't memory leak, but I'd like to check it more in-depth.
		//basicDataType data = 0;
		constexpr const size_t length = sizeof(basicDataType);
		char bytes[length];
		for (size_t i=0;i < length;i++)
		{
			//data |= static_cast<unsigned char>(serialized[i+(*bufferUnserialized)]) << (8*((length-i)-1));
			bytes[((length-i)-1)] = state.serialized[i+(state.bufferUnserialized)];
		}
		state.bufferUnserialized += length;

		//This shouldn't memory leak (allocated on stack) 
		return *reinterpret_cast<basicDataType*>(bytes); 
	}




	//TODO: Support std::array
	/*
	template<typename array_t>
	constexpr void serialize_container_array(array_t array, serialization_state& state) //note, size is how many elements in array, not ammount of memory taken.
	{
		//serialize the length so unserialize knows how many to read
		serialize(size,state.buffer,state.bufferUsed,state.bufferSize);
		for (size_t i =0;i != size;i++)
		{
			serialize(array[i],state.buffer,state.bufferUsed,state.bufferSize); //handles resizing for me
		}
	}

	template<typename array_t>
	constexpr array_t unserialize_container_array(unserialization_state& state)
	{
		//get the size
		size_t size = unserialize<size_t>(state.serialized,state.bufferUnserialized,state.serializedVersion);
		//allocate an array big enough to store all of this (and only that big! users must copy the stuff into their array)
		array_t* array = static_cast<array_t*>(malloc(size));
		//unserialize every element
		for (size_t i=0; i!=size;i++)
		{
			array[i] = unserialize<array_t>(state.serialized,state.bufferUnserialized,state.serializedVersion);
		}
		return array;	
	}
	*/





	template<typename sequence_container>
	constexpr void serialize_container_sequence(sequence_container container,serialization_state& state)
	{
		//serialize the length (else we would not know when to stop, during unserialization)
		serialize(static_cast<size_tp>(container->size()),state);
		for (auto& element : *container)
		{
			serialize(element,state);
		}

	}




	template<typename sequence_container>
	sequence_container unserialize_container_sequence(unserialization_state& state)
	{
		//get length
		size_tp length = unserialize<size_tp>(state);
		sequence_container container = new (typename std::remove_pointer<sequence_container>::type)();
		if constexpr (!(is_list<typename std::remove_pointer<sequence_container>::type>::value || is_set<typename std::remove_pointer<sequence_container>::type>::value))
		{
			container->reserve(length);
		}
		for (size_t i=0;i!=length;i++)
		{
			//insert into the container the next unserialization(of the type that our container is)
			container->insert(container->end(),unserialize<typename std::remove_pointer<sequence_container>::type::value_type>(state));
		}
		return container;
	}





	template<typename associative_container>
	constexpr void serialize_container_associative(associative_container container,serialization_state& state)
	{
		//serialize the length (else we would not know when to stop, during unserialization)
		serialize(static_cast<size_tp>(container->size()),state);
		for (const auto& element : (*container))
		{
			serialize(element.first,state);
			serialize(element.second,state);
		}
	}
	template<typename associative_container>
	associative_container unserialize_container_associative(unserialization_state& state)
	{
		size_tp length = unserialize<size_tp>(state);
		associative_container container = new (typename std::remove_pointer<associative_container>::type)();
		for (size_t i=0;i!=length;i++)
		{
			//insert a pair of the key type, and mapped type. 
			
			//std::make_pair runs the code to get the SECOND key FIRST, thus inversing the map. So, go ahead and make this a one-liner for a nice mindf
			auto first = unserialize<typename std::remove_pointer<associative_container>::type::key_type>(state);
			auto second = unserialize<typename std::remove_pointer<associative_container>::type::mapped_type>(state);
			container->insert(container->end(),std::make_pair(first,second));
		}
		return container;
	}

}
