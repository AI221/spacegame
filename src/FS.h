/*!
 * @file
 * @author Jackson McNeill
 *
 * A small FS abstraction to fill the gaps SDL leaves with its own FS abstraction.
 */
#pragma once

#include "GeneralEngineCPP.h"
#ifdef outdatedOS

# include <windows.h>

#else

#include <sys/types.h>	
#include <dirent.h>

#endif


#include <vector>
#include <string>
#include <exception>

typedef std::vector<char> GE_FileString; 






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
 * TODO: Broken for windows!
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


/*!
 * Gets the position that a file extension is at
 */
unsigned int GE_GetFileExtensionPosition(std::string fullfilename);
unsigned int GE_GetFileExtensionPosition(const char* fullfilename);

/*!
 * Gets the position that the final / in path is at
 */
unsigned int GE_GetParrentDirectoryPosition(std::string fullfilename);
unsigned int GE_GetParrentDirectoryPosition(const char* fullfilename);


/*!
 * Gets file extension. 
 * Example: doc.txt -> txt
 */
std::string GE_GetFileExtension(const char* fullfilename);
std::string GE_GetFileExtension(std::string fullfilename);

/*!
 * Strips the extension off a file
 * Example: doc.txt -> doc
 */
std::string GE_GetFileNoExtension(std::string fullfilename);
std::string GE_GetFileNoExtension(const char* fullfilename);

/*!
 * Gives you the enclosing directory
 *
 * Example: folder/folder2/file -> folder/folder2
 */
std::string GE_GetParrentDirectory(std::string fullfilename);
std::string GE_GetParrentDirectory(const char* fullfilename);

/*!
 * Returns a GE_FileString which has the contents of a file
 */
GE_FileString GE_ReadAllFromFile(std::string fullfilename);
GE_FileString GE_ReadAllFromFile(const char* fullfilename);

/*!
 * Writes to a file and closes it.
 *
 * Will create the file if it doesn't exist, or overwrite it if it does.
 */
void GE_WriteToFile(const char* fullfilename, char* contents, size_t size);
void GE_WriteToFile(const char* fullfilename, GE_FileString contents);
void GE_WriteToFile(std::string fullfilename, GE_FileString contents);
/*!
 * Returns the base file name
 *
 * Example: folder/folder2/fiLe.txt -> fiLe.txt
 */
std::string GE_GetBaseName(std::string fullfilename);
std::string GE_GetBaseName(const char* fullfilename);

/*!
 * Returns a std::string representing the given GE_FileString.
 *
 * Note, this will loose the ability to hold the \0 character.
 */
std::string GE_GetStringFromFileString(GE_FileString filestring);

/*!
 * Returns a char array representing the given GE_FileString
 *
 * Allocates using operator new -- delete it with delete[] 
 */
char* GE_GetCharArrayFromFileString(GE_FileString filestring);

bool GE_TEST_FS();
