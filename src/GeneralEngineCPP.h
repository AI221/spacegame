/*
 * @file
 * @author Jackson McNeill
 */

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
 * 
 * Macro to return if given value is true. Will return the succeeding text.
 */
#define GE_ReturnIf(value) if(value) return 


#endif // __GENERALENGINECPP_INCLUDED
