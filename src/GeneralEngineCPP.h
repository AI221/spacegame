/*!
 * @file
 * @author Jackson McNeill
 *
 * Generic macros and preprocessor definitions (Such as what OS is being compiled for) 
 * Expanded to contain a couple utility functions and a macro-based unit testing system
 */
#pragma once


#include <cstdio>
#include <algorithm>
#include <math.h>
#include <type_traits>

#ifdef GENERAL_ENGINE_CPP_NO_FS
	//fs forward declaration
	namespace filesystem
	{
		void append_to_file(const char* fullfilename, char* contents, size_t size);
	}
#else
	#include "FS.h"
#endif

//Debug?
#define GE_DEBUG




#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#define outdatedOS
#endif

/*!
 * Returns 1 if a number is greater than maxval
 */
#define GE_NoGreaterThan(number,maxval) if(number>=maxval) { return 1; }

/*!
 * Returns NULL if a number is greater than maxval
 */
#define GE_NoGreaterThan_NULL(number,maxval) if(number>=maxval) { return NULL; }


/*!
 * Define likely / unlikely macros (like Linux kernel) (pulled: https://kernelnewbies.org/FAQ/LikelyUnlikely). Only insert the given variable if __NO_BUILTIN_EXPECT
 */
#ifdef __NO_BUILTIN_EXPECT
#define likely(x) (x)
#define unlikely(x) (x)
#else
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif



/*!
 * Define the maximum length a double will be, after conversion to a string
 */
#define __DOUBLE_MAX_STRLENGTH (DBL_MAX_10_EXP+2)


/*!
 * Define force-inline (for compiler compatibility)
 */
#define GE_FORCE_INLINE inline __attribute__((always_inline))


/*!
 * Reduce accuracy
 * Example: GE_ReduceAccuracy(26,25) -> 25 , GE_ReduceAccuracy(24,25) -> 0 , GE_ReduceAccuracy(25,25) ->25 GE_ReduceAccuracy(51,25) ->50
 */
#define GE_ReduceAccuracy(number,accuracyDegree) (  std::floor(number/accuracyDegree)*accuracyDegree  )


/*!
 * Generate a random value between low, high
 */
#define random_range_double(low,high) (static_cast<double>(  ((rand() % (std::abs(low)+high+1)))-low  ))



/*!
 * "Wraps arround", think the Asteroids ship at the edge of the screen. 
 * Examples:
 * wraparround_clamp(10,25)->25 wraparroundclamp(26,25) ->1 wraparroundclamp(50,25)->0
 */
template<typename number_type>
constexpr number_type wraparround_clamp(number_type number,number_type maxval)
{
	number_type maybe_floored_number = number/maxval;
	if constexpr(std::is_floating_point<number_type>::value)
	{
		maybe_floored_number = std::floor(maybe_floored_number);
	}
	return number-((maybe_floored_number)*maxval);
}


#define GE_TEST_INIT() bool passedAll = true; 

#define GE_TEST_RETURN() return passedAll;

typedef bool GE_TEST_TYPE_RETURN;

#define GE_BEGIN_UNIT_TESTS() filesystem::write_to_file("../log/tests.log","");

/*!
 * Log something to the unit/integration test logs with printf syntax
 */
template<typename ...T >
void GE_TEST_Log(T ... args)
{
	printf(args...); 
	char buffer[2048];
	int size = snprintf(buffer,sizeof(buffer),args...);
	filesystem::append_to_file("../log/tests.log",buffer,size);
}


/*!
 * Standard, simple unit testing. Does not test for std's.
 * Make sure to define the variable "passedAll" as bool and initialize to true.
 */
#define GE_TEST_STD(stringify,outtype,out,function,...) \
{\
	outtype expectout = out;\
	outtype testout = function(__VA_ARGS__);\
	bool pass = testout == expectout;\
	passedAll = std::min(pass,passedAll);\
\
	GE_TEST_Log("[%s] %s(%s), wanted: %s, result: %s\n",pass? "PASS" : "FAIL", #function, #__VA_ARGS__,stringify(expectout), stringify(testout));\
}

#define GE_TEST_ASSERT(stringify, variable, equals, op)\
{\
	bool pass = variable op equals;\
	passedAll = std::min(pass,passedAll);\
	\
	GE_TEST_Log("[%s][ASSERTION] %s(=%s) %s %s(=%s)\n",pass? "PASS" : "FAIL", #variable,stringify(variable), pass? #op : "!"#op, #equals, stringify(equals));\
}

#define GE_RUN_UNIT_TEST(function) GE_TEST_Log("-------- BEGIN TEST %s --------\n",#function); GE_TEST_Log("-------- %s %s --------\n",(function())? "PASS" : "FAIL", #function);
/*!
 * Converts a bool to a string
 */
#define GE_StringifyBool(in) (in? "true" : "false")

/*!
 * Converts an std::string to char
 */
#define GE_StringifyString(in) (in.c_str())

#define GE_StringifyNumber(in) (std::to_string(in).c_str())

#define GE_StringifyChar(in) GE_StringifyNumber(in)

#define GE_ReturnInput(in) (in)

template<class number>
constexpr bool GE_IsInRange(number value, number range1, number range2)
{
	if (value >= range1 && value <= range2)
	{
		return true;
	}
	if (value <= range1 && value >= range2)
	{
		return true;
	}
	return false;

}

/*!
 * Returns UNIX time in seconds.miliseconds
 */
double GE_GetUNIXTime();


/*!
 * Starts a performance benchmark. Note: Uses the variable 'startPerformanceTime'
# * Do not use in mainline code, for test purposes only
 */
#define GE_TEST_StartBenchmark() double startPerformanceTime = GE_GetUNIXTime();
/*!
 * Returns the resulting time from the benchmark.
 * Note, there is some degree of inacuracy, because it takes time to get the time
 */
#define GE_TEST_BenchmarkResult() (GE_GetUNIXTime()-startPerformanceTime)




//https://stackoverflow.com/a/3418029/8342477
#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif 
