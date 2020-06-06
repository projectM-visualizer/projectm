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
#include "Preset.hpp"
#include "PresetFactory.hpp"
#include <iostream>
#include <sstream>
#include <set>
#include <fstream>

#ifdef __unix__
extern "C"
{
#include <errno.h>
}
#endif

#ifdef __APPLE__
extern "C"
{
#include <errno.h>
}
#endif

#include <cassert>
#include "fatal.h"

#include "Common.hpp"

PresetLoader::PresetLoader (int gx, int gy, std::string dirname = std::string()) :_dirname ( dirname ), _dir ( 0 )
{
	_presetFactoryManager.initialize(gx,gy);
	// Do one scan
	if ( _dirname != std::string() )
		rescan();
	else
		clear();
}

PresetLoader::~PresetLoader()
{
	if ( _dir )
		closedir ( _dir );
}

void PresetLoader::setScanDirectory ( std::string dirname )
{
	_dirname = dirname;
}


void PresetLoader::rescan()
{
	// std::cerr << "Rescanning..." << std::endl;

	// Clear the directory entry collection
	clear();

	// If directory already opened, close it first
	if ( _dir )
	{
		closedir ( _dir );
		_dir = 0;
	}

	// Allocate a new a stream given the current directory name
	if ( ( _dir = opendir ( _dirname.c_str() ) ) == NULL )
	{
		handleDirectoryError();
		return; // no files loaded. _entries is empty
	}

	struct dirent * dir_entry;
	std::set<std::string> alphaSortedFileSet;
	std::set<std::string> alphaSortedPresetNameSet;

	while ( ( dir_entry = readdir ( _dir ) ) != NULL )
	{
        if (dir_entry->d_name[0] == 0)
          continue;

		std::ostringstream out;
		// Convert char * to friendly string
		std::string filename ( dir_entry->d_name );

		// Verify extension is projectm or milkdrop
		if (!_presetFactoryManager.extensionHandled(parseExtension(filename)))
			continue;

		if ( filename.length() > 0 && filename[0] == '.' )
			continue;

		// Create full path name
		out << _dirname << PATH_SEPARATOR << filename;

		// Add to our directory entry collection
		alphaSortedFileSet.insert ( out.str() );
		alphaSortedPresetNameSet.insert ( filename );

		// the directory entry struct is freed elsewhere
	}

	// Push all entries in order from the file set to the file entries member (which is an indexed vector)
	for ( std::set<std::string>::iterator pos = alphaSortedFileSet.begin();
	        pos != alphaSortedFileSet.end();++pos )
		_entries.push_back ( *pos );

	// Push all preset names in similar fashion
	for ( std::set<std::string>::iterator pos = alphaSortedPresetNameSet.begin();
	        pos != alphaSortedPresetNameSet.end();++pos )
		_presetNames.push_back ( *pos );

	readRatings(_ratings);
	//Fix rating sums to include saved ratings data
	_ratingsSums = std::vector<int>(TOTAL_RATING_TYPES, 3 * _presetNames.size());


	assert ( _entries.size() == _presetNames.size() );
}

void PresetLoader::writeRatings()
{
	std::ofstream ratingsData("ratings.dat");
	std::vector<RatingList> ratings = getPresetRatings();
	std::vector<std::string> presetNames = getPresetNames();
	// More error checking?
	if (ratingsData.is_open())
	{
		// Default to hard cut ratings
		for (int i = 0; i < ratings[HARD_CUT_RATING_TYPE].size(); i++)
		{
			ratingsData << ratings[HARD_CUT_RATING_TYPE][i] << "\n";
			//ratingsData << " " << presetNames[i] << "\n";
		}
	}
	ratingsData.close();
}

void PresetLoader::readRatings(std::vector<RatingList> & ratings)
{
	std::ifstream ratingsData("ratings.dat");
	//If file doesn't exist
	if (ratingsData.fail())
	{
		// Give all presets equal rating of 3
		ratings = std::vector<RatingList>(TOTAL_RATING_TYPES, RatingList(getPresetNames().size(), 3));
	}
	else
	{
		std::cout << "Reading ratings..." << std::endl;
		for (int i = FIRST_RATING_TYPE; i < TOTAL_RATING_TYPES; ++i)
		{
			ratings.push_back(RatingList());
			for (std::string line; std::getline(ratingsData, line);)
			{
				ratings[i].push_back(std::stoi(line));
			}
		}
		ratingsData.close();
	}
}
std::unique_ptr<Preset> PresetLoader::loadPreset ( unsigned int index )  const
{

	// Check that index isn't insane
	assert ( index < _entries.size() );
	return _presetFactoryManager.allocate
		( _entries[index], _presetNames[index] );

}


std::unique_ptr<Preset> PresetLoader::loadPreset ( const std::string & url )  const
{
//    std::cout << "Loading preset " << url << std::endl;

	try {
		/// @bug probably should not use url for preset name
		return _presetFactoryManager.allocate
				(url, url);
	} catch (const std::exception & e) {
		throw PresetFactoryException(e.what());
	} catch (...) {
		throw PresetFactoryException("preset factory exception of unknown cause");
	}
    return std::unique_ptr<Preset>();
}

void PresetLoader::handleDirectoryError()
{

#ifdef WIN32
	std::cerr << "[PresetLoader] warning: errno unsupported on win32 platforms. fix me" << std::endl;
#else

	switch ( errno )
	{
		case ENOENT:
			std::cerr << "[PresetLoader] ENOENT error. The path \"" << this->_dirname << "\" probably does not exist. \"man open\" for more info." << std::endl;
			break;
		case ENOMEM:
			std::cerr << "[PresetLoader] out of memory! Are you running Windows?" << std::endl;
			abort();
		case ENOTDIR:
			std::cerr << "[PresetLoader] directory specified is not a preset directory! Trying to continue..." << std::endl;
			break;
		case ENFILE:
			std::cerr << "[PresetLoader] Your system has reached its open file limit. Trying to continue..." << std::endl;
			break;
		case EMFILE:
			std::cerr << "[PresetLoader] too many files in use by projectM! Bailing!" << std::endl;
			break;
		case EACCES:
			std::cerr << "[PresetLoader] permissions issue reading the specified preset directory." << std::endl;
			break;
		default:
			break;
	}
#endif
}

void PresetLoader::setRating(unsigned int index, int rating, const PresetRatingType ratingType)
{
	const unsigned int ratingTypeIndex = static_cast<unsigned int>(ratingType);
	assert (index < _ratings[ratingTypeIndex].size());

	_ratingsSums[ratingTypeIndex] -= _ratings[ratingTypeIndex][index];

	_ratings[ratingTypeIndex][index] = rating;
	_ratingsSums[ratingType] += rating;

}


unsigned int PresetLoader::addPresetURL ( const std::string & url, const std::string & presetName, const std::vector<int> & ratings)
{
	_entries.push_back(url);
	_presetNames.push_back ( presetName );

	assert(ratings.size() == TOTAL_RATING_TYPES);
	assert(ratings.size() == _ratings.size());

    for (unsigned int i = 0; i < _ratings.size(); i++)
		_ratings[i].push_back(ratings[i]);

    for (unsigned int i = 0; i < ratings.size(); i++)
		_ratingsSums[i] += ratings[i];

	return _entries.size()-1;
}

void PresetLoader::removePreset ( unsigned int index )
{

	_entries.erase ( _entries.begin() + index );
	_presetNames.erase ( _presetNames.begin() + index );

    for (unsigned int i = 0; i < _ratingsSums.size(); i++) {
		_ratingsSums[i] -= _ratings[i][index];
		_ratings[i].erase ( _ratings[i].begin() + index );
	}


}

const std::string & PresetLoader::getPresetURL ( unsigned int index ) const
{
	return _entries[index];
}

const std::string & PresetLoader::getPresetName ( unsigned int index ) const
{
	return _presetNames[index];
}

const std::vector<std::string> &PresetLoader::getPresetNames() const
{
	return _presetNames;
}
	unsigned int PresetLoader::getPresetIndex(std::string &name)
{
	//Need a better structure than a vector for reverse lookup
	for (int index = 0; index < _presetNames.size(); index++)
	{
		if (_presetNames[index] == name) return index;
	}
	return 0;
}

int PresetLoader::getPresetRating ( unsigned int index, const PresetRatingType ratingType ) const
{
	return _ratings[ratingType][index];
}

const std::vector<RatingList> & PresetLoader::getPresetRatings () const
{
	return _ratings;
}

const std::vector<int> & PresetLoader::getPresetRatingsSums() const {
	return _ratingsSums;
}

void PresetLoader::setPresetName(unsigned int index, std::string name) {
	_presetNames[index] = name;
}
void PresetLoader::insertPresetURL ( unsigned int index, const std::string & url, const std::string & presetName, const RatingList & ratings)
{
	_entries.insert ( _entries.begin() + index, url );
	_presetNames.insert ( _presetNames.begin() + index, presetName );



    for (unsigned int i = 0; i < _ratingsSums.size();i++) {
		_ratingsSums[i] += _ratings[i][index];
		_ratings[i].insert ( _ratings[i].begin() + index, ratings[i] );
	}

	assert ( _entries.size() == _presetNames.size() );


}
