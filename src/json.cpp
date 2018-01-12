#include "json.h"


Json::CharReaderBuilder builder;
Json::CharReader* reader;


int GE_JsonInit()
{
	builder = Json::CharReaderBuilder();
	Json::Value settings;
	settings["allowComments"] = true;
	settings["collectComments"] = false;

	builder.setDefaults(&settings);


	reader = builder.newCharReader();


	return 0;


}
 //TODO threadsafeness


int GE_ReadJson(std::string jsonStr, Json::Value* array)
{

	const char* jsonStr_cstr = jsonStr.c_str();
	std::string errorbuffer;

	printf("strt %s fin %s, %lu\n",jsonStr_cstr,jsonStr_cstr+strlen(jsonStr_cstr)-1, strlen(jsonStr_cstr));

	bool parsingSuccessful = reader->parse( jsonStr_cstr,jsonStr_cstr+strlen(jsonStr_cstr)-1, array, &errorbuffer); //start, stop, the array, and error buffer. remember that cstrings are arrays, which are really just pointers.
    if ( !parsingSuccessful )
    {
        std::cout  << "Failed to parse" << errorbuffer << std::endl;
        return 1;
    }

	return 0;

}

void GE_JsonShutdown()
{
	delete reader;
}
