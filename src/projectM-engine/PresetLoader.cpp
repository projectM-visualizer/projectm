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
#include <sstream>
extern "C" {
#include <errno.h>
#include <dirent.h>
}

PresetLoader::PresetLoader(std::string dirname) :m_dirname(dirname)
{
	// Do one scan
	rescan();
}

void PresetLoader::setScanDirectory(std::string dirname) {
	m_dirname = dirname;			
}

void PresetLoader::rescan() {

// Clear the directory entry collection
m_entries.clear();

// If directory already opened, close it first
 if (m_dir) {
	closedir(m_dir);
 } 

// Allocate a new a stream given the current diectory name
if ((m_dir = opendir(m_dirname.c_str())) == NULL) {
	handleDirectoryError();
}

std::string fullPath;
std::ostringstream out(fullPath);

struct dirent * dir_entry;
while ((dir_entry = readdir(m_dir)) != NULL) {

	// Convert char * to friendly string
	std::string filename(dir_entry->d_name);
	
	// Verify extension is projectm or milkdrop
	if (filename.rfind(PROJECTM_FILE_EXTENSION) <= 0
		|| filename.rfind(MILKDROP_FILE_EXTENSION) <= 0)
		continue;

	// Create full path name
	out << m_dirname << PATH_SEPARATOR << filename;

	// Add to our directory entry collection
	m_entries.push_back(out.str());

	// We can now toss out the directory entry struct
	free(dir_entry);
}

}


void PresetLoader::handleDirectoryError() {	
	switch (errno) {
		case ENOENT:
			break;		
		case ENOMEM:
			abort();
		case ENOTDIR:
			break;
		case ENFILE:
			std::cerr << "[PresetLoader] Your system has reached its open file limit. Giving up..." << std::endl;
			abort();
		case EMFILE:
			std::cerr << "[PresetLoader] too many files in use by projectM! Bailing!" << std::endl;
			abort();
		case EACCES:
			break;
		default:
			break;
	}
}