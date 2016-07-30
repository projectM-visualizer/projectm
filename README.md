# Mac OS X Users:
* [Prebuilt iTunes plugin installer](https://github.com/revmischa/projectm/releases)
* [OSX Build instructions](BUILDING_OSX.txt)

# Linux:
* sudo apt-get install cmake-curses-gui libsdl2-dev libglew-dev libgles2-mesa-dev libftgl-dev

# Building
Requires [cmake](https://cmake.org/download/).  
```
cmake .
make
make install
```

## Help
Report issues on [GitHub](https://github.com/revmischa/projectm/issues/new)

## Configure with cmake
If you wish to configure build options run:  
`ccmake .`  
This will present you with a simple console-based interface of options for projectM.  
Review the options and change what you think is necessary. Press 'g' to generate the make files and exit.  
