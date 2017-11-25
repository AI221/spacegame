#include <iostream>
#include "jsoncpp/json/json.h"
#include <string.h>

#ifndef __JSON_INCLUDED
#define  __JSON_INCLUDED


//I don't think this is a thread-safe subsystem

int GE_JsonInit();

int GE_ReadJson(std::string jsonStr, Json::Value* array);


#endif // __JSON_INCLUDED
