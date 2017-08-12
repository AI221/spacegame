/*
Copyright 2017 Jackson Reed McNeill

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "FS.h"
#include <iostream>

std::string GE_ReverseSlashes(std::string victim) 
{
	//sigh... why must windows do everything backwards? In this case, LITERALLY backwards?
	return std::regex_replace(victim, std::regex("/"), std::string("\\"));
}

#ifdef outdatedOS

//SOURCE: https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c#612176
DirList GE_ListInDir(std::string directory) //TODO: Test this. 
{
	std::string folder = GE_ReverseSlashes(directory);
	DirList list;	

	//SO code
	std::vector<std::string> names;
    std::string search_path = folder + "/*.*";
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                names.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
	//end SO code
	
	list.error = 0;
	list.list = names;
}



#else 
DirList GE_ListInDir(std::string directory)
{
	DIR* dir;
	struct dirent *dp;
	DirList list;

	if ((dir = opendir(directory.c_str())) == NULL)
	{
		list.error = 1;
		return list;
	}

	//list.list = std::vector< std::string >; //initialize

	//std::vector<std::string>::iterator it; 
	int it = 0;
	while ((dp = readdir(dir)) != NULL)
	{
		if ( dp->d_name[0] != '.') //TODO kinda shitty because it blocks . prefixed files
		{
			std::string dirStr = dp->d_name;

			list.list.push_back(dirStr);
			list.list[it] = dp->d_name; //dirStr;
		
			std::cout << "c " << std::endl;
			std::cout << list.list[it] << std::endl;


			it++;
		}
	}
	list.error = 0;
	return list;

}
#endif //outdatedOS


char* GE_GetFileExtension(char* fullfilename)
{
	int size, index;
	size = index = 0;

	while(fullfilename[size] != '\0') 
	{
		if(fullfilename[size] == '.') 
		{
			index = size;
		}
		size ++; 
	}

	if(size && index) 
	{
		return fullfilename + index;
	}
	return NULL;
}
