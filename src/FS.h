/*!
 * @file
 * @author Jackson McNeill
 *
 * A small FS abstraction to fill the gaps SDL leaves with its own FS abstraction.
 */
#include "GeneralEngineCPP.h"
#ifdef outdatedOS

#include "windows.h"
#include <stdio.h>
#include <regex>

#else

#include <sys/types.h>	
#include <dirent.h>

#endif

#include <vector>
#include <string>

#ifndef __DIRLIST_INCLUDED
#define __DIRLIST_INCLUDED

/*! 
 * A directory listing. Is not recursive, does not indicate weather any file is a file or folder.
 * If error isn't 0, accessing list is undefined behaviour
 */
struct DirList 
{
	int error;
	std::vector< std::string> list;

};

//SDL2 has a wonderful FS library that can open files, read them, write to them... but not get files in a directory

/*!
 * Returns a DirList for a directory. If the directory doesn't exist, DirList.error will be 1.
 * @param directory A directory that exists. 
 */
DirList GE_ListInDir(std::string directory);
#ifdef outdatedOS
/*!
 * Takes unix file slashes ("/") and converts them to Windows-style slashes ("\"). The inverse is not true, despite the name implying so.
 */
std::string GE_ReverseSlashes(std::string victim);
#endif

#endif //__DIRLIST_INCLUDED

