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
#include <SDL2/SDL.h>
#include <stdio.h>
#include <regex>
#include <algorithm>
#include <new>






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
	for (unsigned int i=0;i<=fullfilename.size();i++)
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
	for (unsigned int i=0;i<=fullfilename.size();i++)
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


struct GE_FileNotFound : std::exception
{
	const char* what() const noexcept { return "File not found"; }
};

const size_t bufferSize = 256;
GE_FileString GE_ReadAllFromFile(const char* fullfilename)
{
	char buffer[bufferSize] = "";
	GE_FileString fullBuffer;
	SDL_RWops *file = SDL_RWFromFile(fullfilename, "rb");
	size_t objsRead;

	if (file == NULL) 
	{
		throw new GE_FileNotFound{};
	}
	else
	{
		size_t fileSize = SDL_RWsize(file);
		size_t remainingBytes = fileSize;

		fullBuffer.reserve(fileSize);  //avoid expensive reallocs
		while (true)
		{	
			objsRead = SDL_RWread(file, buffer, sizeof (buffer), 1);//read as much as our buffer will store

			size_t charsToRead = std::min(remainingBytes,bufferSize); //if there are less bytes remaining to read than what the buffer can store, then only read that many (e.g. 121 instead of 256)

			for (size_t i=0;i!=charsToRead;i++)
			{
				fullBuffer.push_back(buffer[i]);	
			}
			
			if (objsRead == 0)  //last object wasn't full -- next would be empty
			{
				break; 
			}
			
			remainingBytes -= bufferSize; //we've read a full buffer
		}
		SDL_RWclose(file);
	}
	return fullBuffer;
}
GE_FileString GE_ReadAllFromFile(std::string fullfilename)
{
	return GE_ReadAllFromFile(fullfilename.c_str());
}
void GE_WriteToFile(const char* fullfilename, char* contents, size_t size)
{
	SDL_RWops *file = SDL_RWFromFile(fullfilename, "w");
	SDL_RWwrite(file, contents,size,1);
	SDL_RWclose(file);
}
void GE_WriteToFile(const char* fullfilename, GE_FileString contents)
{
	char* buffer = GE_GetCharArrayFromFileString(contents);
	GE_WriteToFile(fullfilename,buffer,static_cast<size_t>(contents.size()));
	delete[] buffer;
}
void GE_WriteToFile(std::string fullfilename, GE_FileString contents)
{
	GE_WriteToFile(fullfilename.c_str(),contents);
}

std::string GE_GetStringFromFileString(GE_FileString filestring)
{
	std::string buffer;
	for (char character : filestring)
	{
		buffer += character;
	}
	return buffer;
}

char* GE_GetCharArrayFromFileString(GE_FileString filestring)
{	
	char* buffer = new char[filestring.size()];
	size_t i = 0;
	for (char character : filestring)
	{
		buffer[i] = character;
		i++;
	}
	return buffer;
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
	

	char teststr[] = "this IS a test write. Null(\0";

	GE_WriteToFile("/tmp/test-file",teststr,sizeof(teststr));

	GE_FileString returnStr = GE_ReadAllFromFile("/tmp/test-file");

	char* returnStrCharStr = GE_GetCharArrayFromFileString(returnStr);

	GE_TEST_Log("Ensure file IO works\n");
	GE_TEST_ASSERT(GE_StringifyChar,returnStrCharStr[11],teststr[11],==);
	GE_TEST_Log("Ensure null character works\n");
	GE_TEST_ASSERT(GE_StringifyChar,returnStrCharStr[27],'\0',==);

	delete[] returnStrCharStr;


	return passedAll;
}
