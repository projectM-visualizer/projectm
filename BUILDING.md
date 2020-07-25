# Building projectM from source

### Dependencies
Relevant for Linux distributions, FreeBSD, macOS:

* `autotools` `autoconf`
* `libtool`, or at least `pkg-config`
* `which`
* `libglm-dev`

Main build options & their requirements:

| Configure flag        | Required dependency                                                                    | Produced binary       |
|-----------------------|----------------------------------------------------------------------------------------|-----------------------|
| `*`                   | `libglm-dev`                                                                           |                       |
| `--enable-sdl`        | `libsdl2-dev`                                                                          | `projectMSDL`         |
| `--enable-pulseaudio` | `qt5-default` `qtdeclarative5-dev` `libpulse-dev` `libqt5opengl5-dev`                  | `projectM-pulseaudio` |
| `--enable-jack`       | `libjack2-dev`OR`libjack1-dev`; `qt5-default` `qtdeclarative5-dev` `libqt5opengl5-dev` | `projectM-jack`       |

#### Additional information on dependencies
* `libglm` (headers only) for matrix math is required.
* A modified version of `hlslparser` is included in Renderer and used to transpile HLSL shaders to GLSL
* OpenGL 3+ or OpenGLES is required
* `libsdl >= 2.0.5` is required for the SDL and emscripten apps. `src/projectM-sdl` is the current reference application implementation. maybe try getting that to build and run as your testbench.

If extra information needed - you can refere to `configure.ac` and the assorted `Makefile.am` files.

### Building process under *nix systems
```sh
./autogen.sh
./configure --enable-sdl    # supply additional options here, info in Dependencies

make
sudo make install
```

### Debian/Ubuntu/Mint
```sh
sudo apt install clang libsdl1.2-dev libsdl2-dev libglm-dev libgl1-mesa-dev qt5-default qtdeclarative5-dev libqt5opengl5-dev libjack-dev libpulse-dev
./configure && make -j4 && sudo make install
```

### OpenGL ES
projectM supports OpenGL ES 3 for embedded systems. Be sure to configure with the `--enable--gles` flag.

### Raspberry Pi (and other embedded systems)
* projectM is arch-independent, although there are some SSE2 enhancements for x86
* [Notes on running on raspberry pi](https://github.com/projectM-visualizer/projectm/issues/115)

### Building on Windows
Windows build bypasses the autogen/configure pipeline and uses the Visual Studio/MSVC files in `msvc/`. See `.appveyor.yml` for command line building.

Some dependencies are included verbatim (glew), while others leverage the NuGet ecosystem and are downloaded automatically (glm, sdl2).

### Build using NDK for Android
Install Android Studio, launch SDK Manager and install NDK

```sh
./autogen.sh
./configure-ndk
make && make install-strip
```

Now you should be able to copy ./src/libprojectM/.libs/libprojectM.so
and appropriate headers to projectm-android, and build it using Android Studio

### LLVM JIT
There are some optmizations for parsing preset equations that leverage the LLVM JIT. You can try `./compile --enable--llvm` to enable them. They may not work with newer version of LLVM (https://github.com/projectM-visualizer/projectm/pull/360)


## libprojectM

`libprojectM` is the core library. It is made up of three sub-libraries:

#### Renderer
Made up of everything in `src/libprojectM/Renderer`. These files compose the `libRenderer` sub-library.

#### MilkdropPresetFactory / NativePresetFactory
From their respective folders. Native presets are visualizations that are implemented in C++ instead of `.milk` preset files. They are completely optional. Milkdrop presets are technically optional but the whole thing is basically useless without them.

If you don't want native presets, and you probably don't, don't bother with them. Ideally there should be a configure option to disable them, probably on by default (at this time this is needed for autoconf: https://github.com/projectM-visualizer/projectm/issues/99).


### Assets
`libprojectM` can either have a configuration hard-coded or load from a configuration file. It's up to each application to decide how to load the config file. The config file can have paths defined specifying where to load fonts and presets from.

You will want to install the config file and presets somewhere, and then define that path for the application you're trying to build.
