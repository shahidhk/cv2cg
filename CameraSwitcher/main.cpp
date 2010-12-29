/* 
 *  Copyright (c) 2010  Chen Feng (cforrest (at) umich.edu)
 *    and the University of Michigan
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

/* main.cpp */

#include <iostream>
#include "CameraSwitcher.h"

int main(int argc, char** argv)
{
#ifdef USE_IN_CHINA
	setlocale(LC_ALL,"chs");
#endif
	if(argc<2) {
		std::cout<<"Usage: "
			<<argv[0]<<" filename"<<std::endl;
		return 1;
	}

	CameraSwitcher cs;
	cs.infilename = std::string(argv[1]);
	cs.writeModel=false;
	if(argc>2) {
		if(std::string("w") == std::string(argv[2]))
			cs.writeModel=true;
	}
	return cs.run();
}