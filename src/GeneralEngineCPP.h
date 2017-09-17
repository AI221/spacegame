/*
 * @file
 * @author Jackson McNeill
 *
 * Generic macros and preprocessor definitions (Such as what OS is being compiled for) 
 */


//Debug?
#define GE_DEBUG


#ifndef __GENERALENGINECPP_INCLUDED
#define  __GENERALENGINECPP_INCLUDED


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



#endif // __GENERALENGINECPP_INCLUDED
