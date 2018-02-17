/*!
 * @file
 * @author Jackson McNeill
 * Minimal wrapper arround the jsoncpp library
 */
#pragma once

#include <iostream>
#include "jsoncpp/json/json.h"
#include <string.h>




//I don't think this is a thread-safe subsystem

int GE_JsonInit();

int GE_ReadJson(std::string jsonStr, Json::Value* array);

void GE_JsonShutdown();

