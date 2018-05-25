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


namespace filesystem
{



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
	DirList ListInDir(std::string directory);
#ifdef outdatedOS
	/*!
	 * Takes unix file slashes ("/") and converts them to Windows-style slashes ("\"). The inverse is not true, despite the name implying so.
	 */
	std::string ReverseSlashes(std::string victim);
#endif


	/*!
	 * Gets the position that a file extension is at
	 */
	unsigned int get_filename_extension_position(std::string fullfilename);
	unsigned int get_filename_extension_position(const char* fullfilename);

	/*!
	 * Gets the position that the final / in path is at
	 */
	unsigned int get_parrent_directory_position(std::string fullfilename);
	unsigned int get_parrent_directory_position(const char* fullfilename);


	/*!
	 * Gets file extension. 
	 * Example: doc.txt -> txt
	 */
	std::string get_filename_extension(const char* fullfilename);
	std::string get_filename_extension(std::string fullfilename);

	/*!
	 * Strips the extension off a file
	 * Example: doc.txt -> doc
	 */
	std::string get_filename_without_extension(std::string fullfilename);
	std::string get_filename_without_extension(const char* fullfilename);

	/*!
	 * Gives you the enclosing directory
	 *
	 * Example: folder/folder2/file -> folder/folder2
	 */
	std::string get_parrent_directory(std::string fullfilename);
	std::string get_parrent_directory(const char* fullfilename);

	/*!
	 * Returns a std::string which has the contents of a file
	 */
	std::string read_all_from_file(std::string fullfilename);
	std::string read_all_from_file(const char* fullfilename);

	/*!
	 * Writes to a file and closes it.
	 *
	 * Will create the file if it doesn't exist, or overwrite it if it does.
	 */
	void write_to_file(const char* fullfilename, char* contents, size_t size);
	void write_to_file(const char* fullfilename, std::string contents);
	void write_to_file(std::string fullfilename, std::string contents);
	/*!
	 * Returns the base file name
	 *
	 * Example: folder/folder2/fiLe.txt -> fiLe.txt
	 */
	std::string get_base_filename(std::string fullfilename);
	std::string get_base_filename(const char* fullfilename);

	bool TEST_FS();
}
