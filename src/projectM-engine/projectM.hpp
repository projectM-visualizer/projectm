/*
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
/**
 * $Id: projectM.hpp,v 1.1.1.1 2005/12/23 18:05:11 psperl Exp $
 *
 * Encapsulation of ProjectM engine
 *
 * $Log$
 */

#ifndef _PROJECTM_H
#define _PROJECTM_H

#ifdef WIN32
#include "win32-dirent.h"
#else
#include <dirent.h>
#endif /** WIN32 */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <sys/types.h>

#ifdef MACOS
#include <MacWindows.h>
#include <gl.h>
#include <glu.h>
#else
#ifdef WIN32
#include <windows.h>
#endif /** WIN32 */

#endif /** MACOS */
#ifdef WIN322
#define inline
#endif /** WIN32 */
#ifndef WIN32
#include <sys/time.h>
#else
#endif /** !WIN32 */

#include "dlldefs.h"
#include "event.h"
#include "fatal.h"
#include "PresetFrameIO.hpp"
#include "PCM.hpp"

#include <memory>

class BeatDetect;
class PCM;
class Func;
class Renderer;
class Preset;
class PresetIterator;
class PresetChooser;
class PresetLoader;

//class PresetInputs;
//class PresetOutputs;

#include <memory>
#ifdef WIN32
#pragma warning (disable:4244)
#pragma warning (disable:4305)
#endif /** WIN32 */

#ifdef MACOS2
#define inline
#endif

/** KEEP THIS UP TO DATE! */
#define PROJECTM_VERSION "1.00.00"
#define PROJECTM_TITLE "projectM 1.00.00"

/** Interface types */
typedef enum {
    MENU_INTERFACE,
    SHELL_INTERFACE,
    EDITOR_INTERFACE,
    DEFAULT_INTERFACE,
    BROWSER_INTERFACE
  } interface_t;


class projectM 
{
public:

  PCM *pcm;
    
  DLLEXPORT projectM(std::string config_file);
  DLLEXPORT projectM(int gx, int gy, int fps, int texsize, int width, int height,std::string preset_url,std::string title_fonturl, std::string title_menuurl);
  
  DLLEXPORT void projectM_resetGL( int width, int height );
  DLLEXPORT void projectM_resetTextures();
  DLLEXPORT void projectM_setTitle( std::string title );
  DLLEXPORT void renderFrame();
  DLLEXPORT unsigned initRenderToTexture(); 
  DLLEXPORT void key_handler( projectMEvent event,
		    projectMKeycode keycode, projectMModifier modifier );
        
  virtual ~projectM();

  /// Plays a preset immediately  
  void selectPreset(unsigned int index);

  /// Removes a preset from the play list. If it is playing then it will continue as normal until next switch
  void removePreset(unsigned int index);
 
  /// Tell projectM to play a particular preset when it chooses to switch
  /// If the preset is locked the queued item will be not switched to until the lock is released
  void queuePreset(unsigned int index);


  /// Removes entire playlist, The currently loaded preset will end up sticking until new presets are added
  void clearPlaylist();

  /// Turn on or off a lock that prevents projectM from switching to another preset
  void setPresetLock(bool isLocked);

  /// Returns true if the active preset is locked
  bool isPresetLocked() const;

  /// Add a preset url to the play list. Appended to bottom
  unsigned int addPresetURL(const std::string & presetURL, const std::string & presetName);

  /// Returns the url associated with a preset index
  std::string getPresetURL(unsigned int index) const;

  /// Returns the preset name associated with a preset index
  std::string getPresetName ( unsigned int index ) const;

  /// Returns the size of the play list
  unsigned int getPlaylistSize() const;

  /// Occurs when active preset has switched. Switched to index is returned
  virtual void presetSwitchedEvent(bool isHardCut, unsigned int index) const {};

private:
   
  BeatDetect * beatDetect;
  Renderer *renderer;
  std::string presetURL;
  
  std::string title_fontURL;
  std::string menu_fontURL;
  
  int smoothFrame;
    
#ifndef WIN32
  /* The first ticks value of the application */
  struct timeval startTime;
#else
  long startTime;
#endif /** !WIN32 */

  int wvw;      //windowed dimensions
  int wvh;
  
  int avgtime;  //# frames per preset
  
  /** Timing information */
  int mspf;
  int timed;
  int timestart;
  int nohard;
  int count;
  float fpsstart;

  void readConfig(std::string config_file);
  void projectM_init(int gx, int gy, int fps, int texsize, int width, int height);
  void projectM_reset();

  void projectM_initengine();
  void projectM_resetengine();
  /// Initializes preset loading / management libraries
  int initPresetTools();
  
  /// Deinitialize all preset related tools. Usually done before projectM cleanup
  void destroyPresetTools();

  void default_key_handler( projectMEvent event, projectMKeycode keycode );

  /// The current position of the directory iterator
  PresetIterator * m_presetPos;
  
  /// Required by the preset chooser. Manages a loaded preset directory
  PresetLoader * m_presetLoader;
  
  /// Provides accessor functions to choose presets
  PresetChooser * m_presetChooser;
  
  /// Currently loaded preset
  std::auto_ptr<Preset> m_activePreset;
  
  /// Destination preset when smooth preset switching
  std::auto_ptr<Preset> m_activePreset2;
    
  /// All readonly variables which are passed as inputs to presets
  PresetInputs presetInputs;
  
  /// A preset outputs container used and modified by the "current" preset
  PresetOutputs presetOutputs;
  
  /// A preset outputs container used for smooth preset switching
  PresetOutputs presetOutputs2;
};
#endif
