# How to build projectM from scratch

There are autoconf and automake files for projectM. It is suggested to read configure.ac and the assorted Makefile.am files.


## libprojectM

libprojectM is the core library. It is made up of three sub-libraries:

#### Renderer
Made up of everything in src/libprojectM/Renderer. It should project libRenderer

#### MilkdropPresetFactory / NativePresetFactory
From their respective folders. Native presets are visualizations that are implemented in C++ instead of .milk preset files. They are completely optional. Milkdrop presets are technically optional but the whole thing is basically useless without them.

If you don't want native presets, and you probably don't, don't bother with them. Ideally there should be a configure option to disable them, probably on by default (at this time this is needed for autoconf: https://github.com/projectM-visualizer/projectm/issues/99).



### Dependencies

* libglm for matrix math is required, just the headers though. We are considering "vendoring" them.
* A modified version of hlslparser is included in Renderer and used to transpile HLSL shaders to GLSL
* OpenGL 3+ or OpenGLES is required
* libftgl was used for drawing text. It is optional and may or may not be dropped
* libglew was used. not sure if it's still used.
* libsdl >= 2.0.5 is required for the SDL and emscripten apps. src/projectM-sdl is the current reference application implementation. maybe try getting that to build and run as your testbench.


#### Assets
libprojectM can either have a configuration hard-coded or load from a configuration file. It's up to each application to decide how to load the config file. The config file can have paths defined specifying where to load fonts and presets from.

You will want to install the config file and presets somewhere, and then define that path for the application you're trying to build.
