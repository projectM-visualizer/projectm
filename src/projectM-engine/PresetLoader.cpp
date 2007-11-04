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
#include <iostream>
#include <sstream>
#include <set>

#ifdef LINUX
extern "C"
{
#include <errno.h>
}
#endif


#include <cassert>
#include "projectM.hpp"

const std::string PresetLoader::PROJECTM_FILE_EXTENSION(".prjm");
const std::string PresetLoader::MILKDROP_FILE_EXTENSION(".milk");

PresetLoader::PresetLoader(std::string dirname) :m_dirname(dirname), m_dir(0)
{
  // Do one scan
  rescan();
}

PresetLoader::~PresetLoader()
{
  if (m_dir)
    closedir(m_dir);
}

void PresetLoader::setScanDirectory(std::string dirname)
{
  m_dirname = dirname;
}


void PresetLoader::rescan()
{
  // std::cerr << "Rescanning..." << std::endl;

  // Clear the directory entry collection
  m_entries.clear();
  m_presetNames.clear();

  // If directory already opened, close it first
  if (m_dir)
  {
    closedir(m_dir);
    m_dir = 0;
  }

  // Allocate a new a stream given the current directory name
  if ((m_dir = opendir(m_dirname.c_str())) == NULL)
  {
    handleDirectoryError();
    return; // no files loaded. m_entries is empty
  }

  struct dirent * dir_entry;
  std::set<std::string> alphaSortedFileSet;
  std::set<std::string> alphaSortedPresetNameSet;
  
  while ((dir_entry = readdir(m_dir)) != NULL)
  { 

    std::ostringstream out;
    // Convert char * to friendly string
    std::string filename(dir_entry->d_name);

    // Verify extension is projectm or milkdrop
    if ((filename.rfind(PROJECTM_FILE_EXTENSION) != (filename.length() - PROJECTM_FILE_EXTENSION.length()))
        && (filename.rfind(MILKDROP_FILE_EXTENSION) != (filename.length() - MILKDROP_FILE_EXTENSION.length())))
      continue;

    if (filename.length() <= MILKDROP_FILE_EXTENSION.length())
	continue;

    if (filename.length() > 0 && filename[0] == '.')
	continue;

    // Create full path name
    out << m_dirname << PATH_SEPARATOR << filename;

    // Add to our directory entry collection
    alphaSortedFileSet.insert(out.str());
    alphaSortedPresetNameSet.insert(filename);

    // the directory entry struct is freed elsewhere
  }

  // Push all entries in order from the file set to the file entries member (which is an indexed vector)
  for (std::set<std::string>::iterator pos = alphaSortedFileSet.begin(); 
	pos != alphaSortedFileSet.end();++pos) 
	m_entries.push_back(*pos);

  // Push all preset names in similar fashion
  for (std::set<std::string>::iterator pos = alphaSortedPresetNameSet.begin(); 
	pos != alphaSortedPresetNameSet.end();++pos) 
	m_presetNames.push_back(*pos);

}

std::auto_ptr<Preset> PresetLoader::loadPreset(unsigned int index, const PresetInputs & presetInputs, PresetOutputs & presetOutputs) const
{

  // Check that index isn't insane
  assert(index >= 0);
  assert(index < m_entries.size());

  // Return a new autopointer to a present
  return std::auto_ptr<Preset>(new Preset(m_entries[index], m_presetNames[index], presetInputs, presetOutputs));
}

void PresetLoader::handleDirectoryError()
{

#ifdef WIN32
	std::cerr << "[PresetLoader] warning: errno unsupported on win32 platforms. fix me" << std::endl;
#else

  switch (errno)
  {
  case ENOENT:
    std::cerr << "[PresetLoader] ENOENT error. The path \"" << this->m_dirname << "\" probably does not exist. \"man open\" for more info." << std::endl;
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


unsigned int PresetLoader::addPresetURL(const std::string & url, const std::string & presetName)  {
	m_entries.push_back(url);
	m_presetNames.push_back(presetName);
	return m_entries.size()-1;
}

void PresetLoader::removePreset(unsigned int index)  {
	std::vector<std::string>::iterator pos = m_entries.begin();
	asser(pos != m_entries.end());

	pos += index;
	m_entries.erase(pos);
	m_presetNames.erase(pos);
}

const std::string & PresetLoader::getPresetURL ( unsigned int index) const {
	return m_entries[index];
}
		
const std::string & PresetLoader::getPresetName ( unsigned int index) const {
	return m_presetNames[index];
}
