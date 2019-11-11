[![Build Status](https://travis-ci.org/projectM-visualizer/projectm.svg?branch=master)](https://travis-ci.org/projectM-visualizer/projectm)


![Logo](https://github.com/projectM-visualizer/projectm/raw/master/web/logo.png)

## projectM - The most advanced open-source music visualizer
projectM is an open-source project that reimplements the esteemed [Winamp Milkdrop](https://en.wikipedia.org/wiki/MilkDrop) by Geiss in a more modern, cross-platform reusable library.

Its purpose in life is to read an audio input and to produce mesmerizing visuals, detecting tempo, and rendering advanced equations into a limitless array of user-contributed visualizations.

### Available For
* [macOS, Linux (binary)](https://github.com/projectM-visualizer/projectm/releases)
* [Windows Store (PC, XBOX, Phone)](https://www.microsoft.com/store/apps/9NDCVH0VCWJN)
* [Android](https://play.google.com/store/apps/details?id=com.psperl.projectM)
* [iOS](https://itunes.apple.com/us/app/projectm-music-visualizer/id530922227?mt=8&ign-mpt=uo%3D4)
* [iTunes plugin (macOS)](https://github.com/projectM-visualizer/projectm/releases/)
* [Qt5](https://www.qt.io/) + [Pulseaudio](https://www.freedesktop.org/wiki/Software/PulseAudio/)
* [ALSA, xmms, winamp, jack](https://sourceforge.net/projects/projectm/files/) (source, unmaintained)

### Download
Get it from the [Releases](https://github.com/projectM-visualizer/projectm/releases) tab
or
Download from the [Windows Store (PC, XBOX, Phone)](https://www.microsoft.com/store/apps/9NDCVH0VCWJN)

### Demo Video
[![](http://img.youtube.com/vi/2dSam8zwSFw/0.jpg)](http://www.youtube.com/watch?v=2dSam8zwSFw "Demo")


### Project Status
You can read more about how it works and the current state of development [here](https://lwn.net/Articles/750152/).

### Presets
The preset files define the visualizations via pixel shaders and Milkdrop-style equations and parameters. Included with projectM are the bltc201, Milkdrop 1 and 2, projectM, tryptonaut and yin collections. You can grab these presets [here](http://spiegelmc.com/pub/projectm_presets.zip).

You can also download an enormous 41,000 preset pack of presets [here](https://mischa.lol/projectM/presets_community.zip) (123MB zipped).


### Also Featured In
[![Kodi](https://github.com/projectM-visualizer/projectm/raw/master/web/kodi.png) Kodi (formerly XBMC)](https://kodi.tv/)

[![Helix](https://github.com/projectM-visualizer/projectm/raw/master/web/helix.jpg) Helix](http://ghostfiregames.com/helixhome.html)


[![Silverjuke](https://github.com/projectM-visualizer/projectm/raw/master/web/silverjuke.png) Silverjuke (FOSS Jukebox)](https://www.silverjuke.net)

[<img src="https://silentradiance.com/demos/projectM_vr/projectm_vr.png" width="200" > Silent Radiance Distance Disco](https://silentradiance.com)


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

![Screenshot](https://silentradiance.com/demos/projectM_vr/projectm_vr.png)
***

# Building from source
See [BUILDING.md](BUILDING.md)

# Using the library
At its core projectM is a library, [libprojectM](src/libprojectM). This library is responsible for parsing presets, analyzing audio PCM data with beat detection and FFT, applying the preset to the audio feature data and rendering the resulting output with openGL. It can render to an openGL context or a texture.

To look at a simple example way of using the library see the [libSDL2 sample code](src/projectM-sdl/projectM_SDL_main.cpp).

There are many other applications that make use of libprojectM and that can be found in the [src](src/) directory.

***

# Todo
* Support for the missing Milkdrop waveforms to get full compatibility with Milkdrop presets.
* Steal cool stuff from the recently-released Milkdrop source.
* Finish emscripten support for building to wasm/webGL for the web.
* Update the [various implementations using libprojectM](src).
* Update downstream projects with new versions.

***

## Help
Report issues on [GitHub](https://github.com/projectM-visualizer/projectm/issues/new)
We're on IRC at irc.freenode.net #projectM

If you would like to help improve this project, either with documentation, code, porting, hardware or anything else please let us know! We gladly accept pull requests and issues.

## Maintainers
If you maintain packages of libprojectM, we are happy to work with you! Please note well:
* The main focus of this project is libprojectM. It's a library that only really depends on OpenGL. The other applications are more like examples and demos.
* This project was converted to use GNU autotools instead of CMake in 2018. This was due to inflexibility of CMake, issues with OSX and SDL2, an unintuitive configuration interface, standardizing on the most common and well-understood build system, a desire to make it easier for others to get up and running and contribute, and other factors too lengthy to expound on here.
* Most of the applications (e.g. `src/projectM-*`) are likely outdated and of less utility than the core library. If you desire to use them or depend on them, please file an issue so we can help update them.
* The "canonical" application for actually viewing the visualizations is now projectM-sdl, based on libSDL2 because it supports audio input and is completely cross-platform.
* This is an open source project! If you don't like something, feel free to contribute improvements!
* Yes, you are looking at the official version. This is not a fork.

## Authors
[Authors](https://github.com/projectM-visualizer/projectm/raw/master/AUTHORS.txt)


## License
[LGPL](https://github.com/projectM-visualizer/projectm/raw/master/LICENSE.txt)
