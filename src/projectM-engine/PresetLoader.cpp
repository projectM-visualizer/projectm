//
// C++ Implementation: PresetLoader
//
// Description: 
//
//
// Author: Carmelo Piccione <carmelo.piccione@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "PresetLoader.hpp"
#include <iostream>
extern "C" {
#include <errno.h>
}

PresetLoader::PresetLoader(std::string pathame) {

	// Initialize a directory watcher
	#ifdef LINUX
	if ((m_notifyFD = inotify_init()) == -1) {
		std::cerr << "[PresetLoader] failed to initialize inotify instance (error " << errno << std::endl;
	}
	#endif

	
}

