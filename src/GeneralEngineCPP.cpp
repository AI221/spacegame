#include "GeneralEngineCPP.h"

#include <sys/time.h>
#include <chrono>


double GE_GetUNIXTime()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	
	return static_cast<double>(tv.tv_sec)+(static_cast<double>(tv.tv_usec)/(static_cast<double>(10e6))); //Add the microseconds divded by 10e6 to end up with seconds.microseconds
	//Static casting intensifies
}
