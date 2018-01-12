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

	closedir(dir);
	list.error = 0;
	return list;

}
#endif //outdatedOS

unsigned int GE_GetFileExtensionPosition(std::string fullfilename)
{
	int position = -1;
	for (int i=0;i<=fullfilename.size();i++)
	{
		if (fullfilename[i] == '.')
		{
			position= i;
		}
	}
	return position;
}
unsigned int GE_GetFileExtensionPosition(const char* fullfilename)
{
	return GE_GetFileExtensionPosition(std::string(fullfilename));
}


unsigned int GE_GetParrentDirectoryPosition(std::string fullfilename)
{
	int position = -1;
	for (int i=0;i<=fullfilename.size();i++)
	{
		if (fullfilename[i] == '/')
		{
			position= i;
		}
	}
	return position;
}
unsigned int GE_GetParrentDirectoryPosition(const char* fullfilename)
{
	return GE_GetParrentDirectoryPosition(std::string(fullfilename));
}


std::string GE_GetFileExtension(std::string fullfilename)
{
	int position = GE_GetFileExtensionPosition(fullfilename);
	return (position==-1)? fullfilename : fullfilename.substr(position+1,fullfilename.size());
}std::string GE_GetFileExtension(const char* fullfilename)
{
	return GE_GetFileExtension(std::string(fullfilename));
}


std::string GE_GetFileNoExtension(std::string fullfilename)
{
	int position = GE_GetFileExtensionPosition(fullfilename);
	return (position==-1)?fullfilename : fullfilename.substr(0,position);
}
std::string GE_GetFileNoExtension(const char* fullfilename)
{
	return GE_GetFileNoExtension(std::string(fullfilename));
}


std::string GE_GetParrentDirectory(std::string fullfilename)
{
	int position = GE_GetParrentDirectoryPosition(fullfilename);
	return position==-1? fullfilename : fullfilename.substr(0,position);
}
std::string GE_GetParrentDirectory(const char* fullfilename)
{
	return GE_GetParrentDirectory(std::string(fullfilename));
}

std::string GE_GetBaseName(std::string fullfilename)
{
	int position = GE_GetParrentDirectoryPosition(fullfilename);
	return position==-1? fullfilename : fullfilename.substr(position+1,fullfilename.size());
}
std::string GE_GetBaseName(const char* fullfilename)
{
	return GE_GetBaseName(std::string(fullfilename));
}


std::string GE_ReadAllFromFile(const char* fullfilename)
{
	char buffer[256] = "";
	std::string fullBuffer = "";
	SDL_RWops *file = SDL_RWFromFile(fullfilename, "r");

	if (file != NULL) 
	{
		int objsRead;
		while (true)
		{	
			objsRead = file->read(file, buffer, sizeof (buffer), 1);//read as much as our buffer will store
			fullBuffer = fullBuffer+std::string(buffer); //append the C++ string with the C-style buffer - not the most effecient but FS access is slow in general and doesn't need to be fast for games
			if (objsRead == 0) 
			{
				break; //we're done, break the loop
			}
		}
		file->close(file);
	}
	return fullBuffer;
}
std::string GE_ReadAllFromFile(std::string fullfilename)
{
	return GE_ReadAllFromFile(fullfilename.c_str());
}





bool GE_TEST_FS() //mostly tests string manipulation
{
	bool passedAll = true;
	GE_TEST_STD(GE_StringifyString,std::string,"my.file",GE_GetFileNoExtension,"my.file.ext");
	GE_TEST_STD(GE_StringifyString,std::string,"ext",GE_GetFileExtension,"my.file.ext");
	GE_TEST_STD(GE_StringifyString,std::string,"folder",GE_GetParrentDirectory,"folder/file");
	GE_TEST_STD(GE_StringifyString,std::string,"file",GE_GetBaseName,"folder/file");

	GE_TEST_STD(GE_StringifyString,std::string,"file",GE_GetFileNoExtension,"file");
	GE_TEST_STD(GE_StringifyString,std::string,"file",GE_GetFileExtension,"file");
	GE_TEST_STD(GE_StringifyString,std::string,"folder",GE_GetParrentDirectory,"folder");
	GE_TEST_STD(GE_StringifyString,std::string,"file",GE_GetBaseName,"file");

	return passedAll;
}
