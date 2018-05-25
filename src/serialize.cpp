#include "serialize.h"

#include <typeinfo>
#include "GeneralEngineCPP.h"









//Tests:

#include "vector2.h"
#include <vector>
#include <set>
#include <map>

class serialize_test
{
	public:
		serialize_test (int number,int number2)
		{
			myintvector.push_back(number);
			myintvector.push_back(number2);
		}
		constexpr static void serialize(serialize_test* me, serialization::serialization_state& state)
		{
			serialization::serialize(&me->myintvector,state);
		}
		static serialize_test* unserialize(serialization::unserialization_state& state)
		{
			std::vector<int>* t = serialization::unserialize<std::vector<int>*>(state);
			return new serialize_test (*t->begin(),*(t->begin()+1));
		}
		int returnNumber1 ()
		{
			return *myintvector.begin();
		}
		int returnNumber2 ()
		{
			return *(myintvector.begin()+1);
		}

		std::vector<int> myintvector;
	private:
};



namespace serialization
{
	GE_TEST_TYPE_RETURN unit_test()
	{


		GE_TEST_INIT();


		/////////////////////////////////////////////////////////////////0 -- Testing basic datatype serialization
		{
			
			serialization::serialization_state& state = *new serialization::serialization_state(0);


			serialization::serialize((unsigned int)0xffffffff,state);
			serialization::serialize((unsigned int)672214,state);
			serialization::serialize((int)-672214,state);
			serialization::serialize((double)64563.33f,state);

			serialization::unserialization_state& state_u = *new serialization::unserialization_state(state.buffer);



			GE_TEST_STD(GE_StringifyNumber,unsigned int,0xffffffff,serialization::unserialize<unsigned int>,state_u);
			GE_TEST_STD(GE_StringifyNumber,unsigned int,672214,serialization::unserialize<unsigned int>,state_u);
			GE_TEST_STD(GE_StringifyNumber,int,-672214,serialization::unserialize<int>,state_u);
			GE_TEST_STD(GE_StringifyNumber,double,64563.33f,serialization::unserialize<double>,state_u);
		}

		/////////////////////////////////////////////////////////////////1 -- Testing buffer resizing
		{

			serialization::serialization_state& state = *new serialization::serialization_state(0);

			state.bufferSize=3; //dont bother actually changing the size of the char*
			auto versionSize = sizeof(int);
			serialization::serialize((unsigned int)0xffffffff,state);

			GE_TEST_Log("Ensure resizing of buffer works\n");
			GE_TEST_ASSERT(GE_StringifyNumber,state.bufferUsed, 4+versionSize, ==);
			GE_TEST_ASSERT(GE_StringifyNumber,state.bufferSize, 4, >=);
		}

		/////////////////////////////////////////////////////////////////3  -- testing vector
		{

			serialization::serialization_state& state = *new serialization::serialization_state(0);

			std::vector<int> preserialized_vector;
			preserialized_vector.insert(preserialized_vector.end(),27);
			preserialized_vector.insert(preserialized_vector.end(),26);

			serialization::serialize(&preserialized_vector,state);


			serialization::unserialization_state& state_u = *new serialization::unserialization_state(state.buffer);


			std::vector<int>* unserialized_vector = serialization::unserialize<std::vector<int>*>(state_u);

			GE_TEST_Log("Ensure vector unserialized properly\n");
			GE_TEST_ASSERT(GE_StringifyNumber,*unserialized_vector->begin(),*preserialized_vector.begin(),==);
			GE_TEST_Log("Ditto\n");
			GE_TEST_ASSERT(GE_StringifyNumber,*(unserialized_vector->begin()+1),*(preserialized_vector.begin()+1),==);

			delete unserialized_vector;

			
		}
		/////////////////////////////////////////////////////////////////4 -- testing classes
		{
		
			serialization::serialization_state& state = *new serialization::serialization_state(994645232);

			auto myclass = new serialize_test(1337,88);


			serialization::serialize(myclass,state);

			delete myclass;


			serialization::unserialization_state& state_u = *new serialization::unserialization_state(state.buffer);

			GE_TEST_Log("Ensure serialization versioning works properly\n");
			GE_TEST_ASSERT(GE_StringifyNumber,state_u.serializedVersion,994645232,==);

			serialize_test* unserializedclass = (serialization::unserialize<serialize_test*>(state_u));

			GE_TEST_Log("Ensure unserialization worked\n");
			GE_TEST_ASSERT(GE_StringifyNumber,unserializedclass->returnNumber1(),1337,==);
			GE_TEST_ASSERT(GE_StringifyNumber,unserializedclass->returnNumber2(),88,==);

			delete unserializedclass;
		}

		/////////////////////////////////////////////////////////////////5 -- A vector of a class w/ a vector of ints
		{
			serialization::serialization_state& state = *new serialization::serialization_state(0);
		
			std::vector<serialize_test*>* classvec = new std::vector<serialize_test*>;
			classvec->push_back(new serialize_test(1,2));
			classvec->push_back(new serialize_test(3,4));

			serialization::serialize(classvec,state);	

			serialization::unserialization_state& state_u = *new serialization::unserialization_state(state.buffer);

			std::vector<serialize_test*>* unser_classvec = serialization::unserialize<std::vector<serialize_test*>*>(state_u);

			GE_TEST_Log("Ensure unserialization of vector of serializable class worked\n");
			GE_TEST_ASSERT(GE_StringifyNumber,(*unser_classvec->begin())->returnNumber1(),1,==);
			GE_TEST_ASSERT(GE_StringifyNumber,(*unser_classvec->begin())->returnNumber2(),2,==);
			GE_TEST_ASSERT(GE_StringifyNumber,(*(unser_classvec->begin()+1))->returnNumber1(),3,==);
			GE_TEST_ASSERT(GE_StringifyNumber,(*(unser_classvec->begin()+1))->returnNumber2(),4,==);


			delete classvec;
		}

		/////////////////////////////////////////////////////////////////6 -- testing map containers
		{
			serialization::serialization_state& state = *new serialization::serialization_state(0);

			std::map<int,int>* maptest = new std::map<int,int>;

			maptest->insert(std::make_pair(10,20));
			maptest->insert(std::make_pair(30,40));

			serialization::serialize(maptest,state);


			serialization::unserialization_state& state_u = *new serialization::unserialization_state(state.buffer);

			auto unser_map = serialization::unserialize<std::map<int,int>*>(state_u);

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

			delete maptest;
			delete unser_map;

		}
		
		/////////////////////////////////////////////////////////////////7 -- testing empty vector
		{
			serialization::serialization_state& state = *new serialization::serialization_state(0);

			std::vector<int> preserialized_empty_vector;
			serialization::serialize(&preserialized_empty_vector,state);

			serialization::unserialization_state& state_u = *new serialization::unserialization_state(state.buffer);

			std::vector<int>* unserialized_empty_vector = serialization::unserialize<std::vector<int>*>(state_u);

			GE_TEST_Log("Ensure empty vector unserialized properly\n");
			GE_TEST_ASSERT(GE_StringifyNumber,unserialized_empty_vector->size(),0,==);
			delete unserialized_empty_vector;
		}

		/////////////////////////////////////////////////////////////////8 -- testing list containers
		{

			serialization::serialization_state& state = *new serialization::serialization_state(0);

			std::list<int> preserialized_list;
			preserialized_list.insert(preserialized_list.end(),27);
			preserialized_list.insert(preserialized_list.end(),26);

			serialization::serialize(&preserialized_list,state);

			serialization::unserialization_state& state_u = *new serialization::unserialization_state(state.buffer);

			std::list<int>* unserialized_list = serialization::unserialize<std::list<int>*>(state_u);

			GE_TEST_Log("Ensure list unserialized properly\n");
			GE_TEST_ASSERT(GE_StringifyNumber,*unserialized_list->begin(),*preserialized_list.begin(),==);
			GE_TEST_Log("Ditto\n");
			//GE_TEST_ASSERT(GE_StringifyNumber,*(unserialized_list->begin()+1),*(preserialized_list.begin()+1),==);

			delete unserialized_list;


		}
		/////////////////////////////////////////////////////////////////9 -- testing set containers
		{
			

			serialization::serialization_state& state = *new serialization::serialization_state(0);
			
			std::set<int>* settest = new std::set<int>;

			settest->insert(1);
			settest->insert(2);

			serialization::serialize(settest,state);

			serialization::unserialization_state& state_u = *new serialization::unserialization_state(state.buffer);

			auto unser_set = serialization::unserialize<std::set<int>*>(state_u);

			GE_TEST_Log("Ensure sets work\n");
			GE_TEST_ASSERT(GE_StringifyNumber,*(unser_set->begin()),1,==);

			int i=1;
			for (const auto &element : (*unser_set))
			{
				GE_TEST_ASSERT(GE_StringifyNumber,element,i,==);
				i++;
			}
			delete settest;
		}

		/////////////////////////////////////////////////////////////////10 - serializing a rectangle
		{
			serialization::serialization_state& state = *new serialization::serialization_state(0);
			
			GE_Rectangle rect = GE_Rectangle{1,2,3,4};
			
			serialization::serialize(&rect,state);

			serialization::unserialization_state& state_u = *new serialization::unserialization_state(state.buffer);

			auto unser_rect = serialization::unserialize<GE_Rectangle*>(state_u);

			GE_TEST_Log("Ensure GE_Rectangle serialization worked\n");
			GE_TEST_ASSERT(GE_StringifyNumber,unser_rect->x,rect.x,==);
			GE_TEST_ASSERT(GE_StringifyNumber,unser_rect->y,rect.y,==);
			GE_TEST_ASSERT(GE_StringifyNumber,unser_rect->w,rect.w,==);
			GE_TEST_ASSERT(GE_StringifyNumber,unser_rect->h,rect.h,==);

			delete unser_rect;

		}


		GE_TEST_RETURN();
	}
}
