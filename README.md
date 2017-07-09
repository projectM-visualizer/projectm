![Logo](https://github.com/projectM-visualizer/projectm/raw/master/web/logo.png)

## projectM - The most advanced open-source music visualizer
projectM is an open-source project that reimplements the esteemed [Winamp Milkdrop](https://en.wikipedia.org/wiki/MilkDrop) by Geiss in a more modern, cross-platform reusable library.

Its purpose in life is to read in audio input and produce mesmerizing visuals, detecting tempo, and rendering advanced equations into a limitless array of user-contributed visualizations.

### Available For
* [Mac OS X iTunes (plugin)](https://github.com/projectM-visualizer/projectm/releases)
* [Android](https://play.google.com/store/apps/details?id=com.psperl.projectM)
* [iOS](https://itunes.apple.com/us/app/projectm-music-visualizer/id530922227?mt=8&ign-mpt=uo%3D4)
* [ALSA, Qt, xmms, winamp, jack, pulseaudio](https://sourceforge.net/projects/projectm/files/)

### Presets
The preset files define the visualizations via pixel shaders and Milkdrop-style equations and parameters. Included with projectM are the bltc201, Milkdrop 1 and 2, projectM, tryptonaut and yin collections. You can grab all of the presets [here](http://spiegelmc.com.s3.amazonaws.com/pub/projectm_presets.zip).

### Also Featured In
[![Kodi](https://github.com/projectM-visualizer/projectm/raw/master/web/kodi.png)](https://kodi.tv/)  
Kodi (formerly XBMC)  

[![Helix](https://github.com/projectM-visualizer/projectm/raw/master/web/helix.jpg)](http://ghostfiregames.com/helixhome.html)  
Helix

***

## Screenshots
![Screenshot](https://github.com/projectM-visualizer/projectm/raw/master/src/projectM-iTunes/projectM%20screenshots/Screen%20Shot%202014-08-25%20at%2012.31.20%20AM.png)

![Screenshot](https://github.com/projectM-visualizer/projectm/raw/master/src/projectM-iTunes/projectM%20screenshots/Screen%20Shot%202014-08-25%20at%2012.33.50%20AM.png)

![Screenshot](https://github.com/projectM-visualizer/projectm/raw/master/src/projectM-iTunes/projectM%20screenshots/Screen%20Shot%202014-07-18%20at%202.14.41%20PM.png)

![Screenshot](https://github.com/projectM-visualizer/projectm/raw/master/src/projectM-iTunes/projectM%20screenshots/Screen%20Shot%202014-07-18%20at%202.13.53%20PM.png)

![Screenshot](https://github.com/projectM-visualizer/projectm/raw/master/src/projectM-iTunes/projectM%20screenshots/Screen%20Shot%202014-07-18%20at%202.15.36%20PM.png)

![Screenshot](https://github.com/projectM-visualizer/projectm/raw/master/src/projectM-iTunes/projectM%20screenshots/Screen%20Shot%202014-08-16%20at%204.49.32%20PM.png)

![Screenshot](https://github.com/projectM-visualizer/projectm/raw/master/src/projectM-iTunes/projectM%20screenshots/Screen%20Shot%202014-08-16%20at%204.50.37%20PM.png)

![Screenshot](https://github.com/projectM-visualizer/projectm/raw/master/src/projectM-iTunes/projectM%20screenshots/Screen%20Shot%202014-08-25%20at%2012.31.07%20AM.png)

***

# Building From Source

## Mac OS X
* [Prebuilt iTunes plugin installer](https://github.com/projectM-visualizer/projectm/releases)
* [OSX Build instructions](https://github.com/projectM-visualizer/projectm/raw/master/BUILDING_OSX.txt)

## Linux
* `sudo apt-get install cmake-curses-gui libsdl2-dev libglew-dev libftgl-dev libsdl2-dev libdevil-dev`

## Building From Source
Requires [cmake](https://cmake.org/download/).  
```
cmake .
make
make install
```

## Configure With cmake
If you wish to configure build options run:  
`ccmake .`  
This will present you with a simple console-based interface of options for projectM.  
Review the options and change what you think is necessary. Press 'g' to generate the make files and exit.  
***

# Using the library
At its core projectM is a library, [libprojectM](tree/master/src/libprojectM). This library is responsible for parsing presets, analyzing audio PCM data with beat detection and FFT, applying the preset to the audio feature data and rendering the resulting output with openGL. It can render to an openGL context or a texture.

For an example of a very simple example use of the library see the [libSDL2 sample code](blob/master/src/projectM-sdl/projectM_SDL_main.cpp).

There are many other applications that make use of libprojectM that can be found in the [src](tree/master/src/) directory.

***

# Todo
* Top priority has for a long time been to [port the calls from OpenGL immediate-mode (old-school) to be compatible with OpenGL ES](issues/11), using [vertex buffer objects](http://duriansoftware.com/joe/An-intro-to-modern-OpenGL.-Chapter-2.1:-Buffers-and-Textures.html). There's a [branch](tree/gles) and [todo list of function calls to replace](blob/gles/glES-porting.txt).
* [macOS Sierra broke the iTunes plugin somehow](https://github.com/projectM-visualizer/projectm/issues/7)
* Build a [libSDL2-based app](https://github.com/projectM-visualizer/projectm/tree/master/src/projectM-sdl) that can use audio from any input device and visualize it. Maybe using [portaudio](http://www.portaudio.com/) or the super new [audio capture support](https://wiki.libsdl.org/SDL_OpenAudioDevice) in libSDL 2.0.5.
* Currently shader support is done via nVidia's Cg shader toolkit. This is ancient and no longer supported and very lame. Use standard OpenGL facilities for compiling and executing the shader-based presets.
* Support for the missing Milkdrop waveforms to get full compatibility with Milkdrop presets.
* Steal cool stuff from the recently-released Milkdrop source.
* Emscripten (transpile to JavaScript and use WebGL) support. [Some of the work is done](tree/emscripten) on this but it requires OpenGL ES (see item 1).
* Update the [various applications using libprojectM](tree/master/src).
* Getting rid of CMake would be awesome. CMake sucks. [build](https://github.com/c3d/build) could be nice.

***

## Help
Report issues on [GitHub](https://github.com/projectM-visualizer/projectm/issues/new)

## Authors
[Authors](https://github.com/projectM-visualizer/projectm/raw/master/AUTHORS.txt)  
If you would like to help improve this project, either with documentation, code, porting, hardware or anything else please let us know! We gladly accept pull requests and issues. 

## License
[LGPL](https://github.com/projectM-visualizer/projectm/raw/master/LICENSE.txt)
