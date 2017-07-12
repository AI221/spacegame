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

