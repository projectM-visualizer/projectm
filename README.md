![Logo](web/logo.png)

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
* [Kodi (formerly XBMC) ![Kodi](web/kodi.png)](https://kodi.tv/)
* [Helix ![Kodi](web/helix.jpg)](http://ghostfiregames.com/helixhome.html)

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
* [OSX Build instructions](BUILDING_OSX.txt)

## Linux
* `sudo apt-get install cmake-curses-gui libsdl2-dev libglew-dev libftgl-dev libsdl2-dev`

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

## Help
Report issues on [GitHub](https://github.com/projectM-visualizer/projectm/issues/new)

## Authors
[Authors](AUTHORS.txt)  
If you would like to help improve this project, either with documentation, code, porting, hardware or anything else please let us know! We gladly accept pull requests and issues. 

## License
[LGPL](LICENSE.txt)
