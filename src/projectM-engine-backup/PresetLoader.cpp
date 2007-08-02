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
extern "C"
{
#include <errno.h>
#include <dirent.h>
}
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
  std::cerr << "Rescanning..." << std::endl;

  // Clear the directory entry collection
  m_entries.clear();

  std::cerr << "cleared!" << std::endl;

  // If directory already opened, close it first
  if (m_dir)
  {
  std::cerr << "m_dir" << std::endl;
    closedir(m_dir);
    m_dir = 0;
  }

  std::cerr << "opening " << m_dirname << std::endl;
  // Allocate a new a stream given the current directory name
  if ((m_dir = opendir(m_dirname.c_str())) == NULL)
  {
    handleDirectoryError();
    abort();
  }

  struct dirent * dir_entry;
  
  while ((dir_entry = readdir(m_dir)) != NULL)
  {

    

    std::ostringstream out;
    // Convert char * to friendly string
    std::string filename(dir_entry->d_name);

    
    // Verify extension is projectm or milkdrop
    if ((filename.rfind(PROJECTM_FILE_EXTENSION) != (filename.length() - PROJECTM_FILE_EXTENSION.length()))
        && (filename.rfind(MILKDROP_FILE_EXTENSION) != (filename.length() - MILKDROP_FILE_EXTENSION.length())))
      continue;

    // Create full path name
    out << m_dirname << PATH_SEPARATOR << filename;

   // std::cerr << "[PresetLoader]" << filename << std::endl;
    // Add to our directory entry collection
    m_entries.push_back(out.str());

    // the directory entry struct is freed elsewhere
  }

}

std::auto_ptr<Preset> PresetLoader::loadPreset(unsigned int index, const PresetInputs & presetInputs, PresetOutputs & presetOutputs) const
{

  // Check that index isn't insane
  assert(index >= 0);
  assert(index < m_entries.size());

  // Return a new auto pointer to a present
  return std::auto_ptr<Preset>(new Preset(m_entries[index], presetInputs, presetOutputs));
}



void PresetLoader::handleDirectoryError()
{
  switch (errno)
  {
  case ENOENT:
    std::cerr << "[PresetLoader] ENOENT error. \"man fopen\" for more info." << std::endl;
    break;
  case ENOMEM:
    std::cerr << "[PresetLoader] out of memory! Are you running Windows?" << std::endl;
    abort();
  case ENOTDIR:
    std::cerr << "[PresetLoader] directory specified is not a preset directory! Cannot continue." << std::endl;
    break;
  case ENFILE:
    std::cerr << "[PresetLoader] Your system has reached its open file limit. Giving up..." << std::endl;
    abort();
  case EMFILE:
    std::cerr << "[PresetLoader] too many files in use by projectM! Bailing!" << std::endl;
    abort();
  case EACCES:
    std::cerr << "[PresetLoader] permissions issue reading the specified preset directory." << std::endl;
    break;
  default:
    break;
  }
}
