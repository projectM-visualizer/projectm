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
#include <sys/types.h>
#include <cassert>
#include "fatal.h"
#include "Common.hpp"

PresetLoader::PresetLoader (int gx, int gy, std::string dirname = std::string()) :_dirname ( dirname )
{
    _presetFactoryManager.initialize(gx,gy);

    std::vector<std::string> dirs{_dirname};
    std::vector<std::string> extensions = _presetFactoryManager.extensionsHandled();
    fileScanner = FileScanner(dirs, extensions);

	// Do one scan
	if ( _dirname != std::string() )
		rescan();
	else
		clear();
}

PresetLoader::~PresetLoader() {}

void PresetLoader::setScanDirectory ( std::string dirname )
{
	_dirname = dirname;
}

void PresetLoader::addScannedPresetFile(const std::string &path, const std::string &name) {
    auto ext = parseExtension(path);
    if (ext.empty())
        return;
    
    ext = "." + ext;

    // Verify extension is projectm or milkdrop
    if (!_presetFactoryManager.extensionHandled(ext))
        return;
    
//    std::cout << "Loading preset file " << path << std::endl;
    
    _entries.push_back(path);
    _presetNames.push_back(name + ext);
}

void PresetLoader::rescan()
{
	// std::cerr << "Rescanning..." << std::endl;
    
    // Clear the directory entry collection
    clear();

    // scan for presets
    using namespace std::placeholders;
    fileScanner.scan(std::bind(&PresetLoader::addScannedPresetFile, this, _1, _2));

    // Give all presets equal rating of 3 - why 3? I don't know
    _ratings = std::vector<RatingList>(TOTAL_RATING_TYPES, RatingList( _presetNames.size(), 3 ));
    _ratingsSums = std::vector<int>(TOTAL_RATING_TYPES, 3 *_presetNames.size());

    assert ( _entries.size() == _presetNames.size() );
}

std::unique_ptr<Preset> PresetLoader::loadPreset ( PresetIndex index )  const
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

void PresetLoader::setRating(PresetIndex index, int rating, const PresetRatingType ratingType)
{
	const unsigned int ratingTypeIndex = static_cast<unsigned int>(ratingType);
	assert (index < _ratings[ratingTypeIndex].size());

	_ratingsSums[ratingTypeIndex] -= _ratings[ratingTypeIndex][index];

	_ratings[ratingTypeIndex][index] = rating;
	_ratingsSums[ratingType] += rating;
}

unsigned long PresetLoader::addPresetURL ( const std::string & url, const std::string & presetName, const std::vector<int> & ratings)
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

void PresetLoader::removePreset ( PresetIndex index )
{
	_entries.erase ( _entries.begin() + index );
	_presetNames.erase ( _presetNames.begin() + index );

    for (unsigned int i = 0; i < _ratingsSums.size(); i++) {
		_ratingsSums[i] -= _ratings[i][index];
		_ratings[i].erase ( _ratings[i].begin() + index );
	}
}

const std::string & PresetLoader::getPresetURL ( PresetIndex index ) const
{
	return _entries[index];
}

const std::string & PresetLoader::getPresetName ( PresetIndex index ) const
{
	return _presetNames[index];
}


// Get vector of preset names
const std::vector<std::string> &PresetLoader::getPresetNames() const
{
	return _presetNames;
}

// Get the preset index given a name
unsigned int PresetLoader::getPresetIndex(const std::string& name) const
{
	return find(_presetNames.begin(), _presetNames.end(), name) - _presetNames.begin();
}

int PresetLoader::getPresetRating ( PresetIndex index, const PresetRatingType ratingType ) const
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

void PresetLoader::setPresetName(PresetIndex index, std::string name) {
	_presetNames[index] = name;
}

void PresetLoader::insertPresetURL ( PresetIndex index, const std::string & url, const std::string & presetName, const RatingList & ratings)
{
	_entries.insert ( _entries.begin() + index, url );
	_presetNames.insert ( _presetNames.begin() + index, presetName );

    for (unsigned int i = 0; i < _ratingsSums.size();i++) {
		_ratingsSums[i] += _ratings[i][index];
		_ratings[i].insert ( _ratings[i].begin() + index, ratings[i] );
	}

	assert ( _entries.size() == _presetNames.size() );
}
