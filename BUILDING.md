# Building projectM from source
When building on *nix and OSX, one uses the autogen/configure workflow. It is suggested to read configure.ac and the assorted Makefile.am files.


```
./autogen.sh  # only needed if this is a git clone
./configure --enable-sdl
make
sudo make install
```

### Linux (debian/ubuntu) dependencies
* `sudo apt install autoconf libtool libsdl2-dev libglm-dev qt5-default qtdeclarative5-dev libqt5opengl5-dev libjack-dev libpulse-dev libsdl-dev`

### FreeBSD dependencies
* `pkg install gcc autoconf automake libtool mesa-libs libGLU sdl2 glm`

### Mac OS X dependencies
* `brew install glm sdl2 automake libtool pkg-config`

### Building on Windows
Windows build bypasses the autogen/configure pipeline and uses the Visual Studio/MSVC files in `msvc/`. See `.appveyor.yml` for command line building.

Some dependencies are included verbatim (glew), while others leverage the NuGet ecosystem and are downloaded automatically (glm, sdl2).

### OpenGL ES
projectM supports OpenGL ES 3 for embedded systems. Be sure to configure with the `--enable--gles` flag.

### Raspberry Pi (and other embedded systems)
* projectM is arch-independent, although there are some SSE2 enhancements for x86
* [Notes on running on raspberry pi](https://github.com/projectM-visualizer/projectm/issues/115)

### Build using NDK for Android
Install Android Studio, launch SDK Manager and install NDK
`./autogen.sh`
`./configure-ndk`
`make && make install-strip`

Now you should be able to copy ./src/libprojectM/.libs/libprojectM.so
and appropriate headers to projectm-android, and build it using Android Studio


## libprojectM

libprojectM is the core library. It is made up of three sub-libraries:

#### Renderer
Made up of everything in src/libprojectM/Renderer. These files compose the libRenderer sub-library.

#### MilkdropPresetFactory / NativePresetFactory
From their respective folders. Native presets are visualizations that are implemented in C++ instead of .milk preset files. They are completely optional. Milkdrop presets are technically optional but the whole thing is basically useless without them.

If you don't want native presets, and you probably don't, don't bother with them. Ideally there should be a configure option to disable them, probably on by default (at this time this is needed for autoconf: https://github.com/projectM-visualizer/projectm/issues/99).


### Dependencies
* libglm for matrix math is required, just the headers though. We are considering "vendoring" them.
* A modified version of hlslparser is included in Renderer and used to transpile HLSL shaders to GLSL
* OpenGL 3+ or OpenGLES is required
* libsdl >= 2.0.5 is required for the SDL and emscripten apps. src/projectM-sdl is the current reference application implementation. maybe try getting that to build and run as your testbench.


### Assets
libprojectM can either have a configuration hard-coded or load from a configuration file. It's up to each application to decide how to load the config file. The config file can have paths defined specifying where to load fonts and presets from.

You will want to install the config file and presets somewhere, and then define that path for the application you're trying to build.
