#include <sys/types.h>  
#include <dirent.h>
#include <vector>
#include <string>

#ifndef __DIRLIST_INCLUDED
#define __DIRLIST_INCLUDED

struct DirList 
{
	int error;
	std::vector< std::string> list;

};

//SDL2 has a wonderful FS library that can open files, read them, write to them... but not get files in a directory
DirList GE_ListInDir(std::string directory);

#endif //__DIRLIST_INCLUDED
