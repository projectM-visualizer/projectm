# Building projectM from source

Since version 4.0, projectM uses CMake to configure and build the library on all platforms.

This file only contains quick-start build instructions. For a more detailed guide and a list of all build options,
please read the [BUILDING-cmake.md](BUILDING-cmake.md) file.

## Quick Start (Debian / Ubuntu)

For other operating systems (Windows/macOS), see the OS-specific sections below.

### Install the build tools and dependencies

Mandatory packages:

```bash
sudo apt install build-essential cmake libgl1-mesa-dev mesa-common-dev
```

**Important:** Depending on your distribution, the CMake package might be too old to build projectM, e.g. Debian 11 (
bookworm) only provides CMake 3.18 while CMake 3.21 is required. In this case, get the latest CMake
release [from Kitware's download page](https://cmake.org/download/).

Optional packages:

```bash
sudo apt install ninja-build # To build projectM with Ninja instead of make
sudo apt install libsdl2-dev # To build the development test UI
```

### Download the projectM sources

For production use, it is highly recommended to use the latest stable version of projectM. Download the latest release
from the [Releases page on GitHub](https://github.com/projectM-visualizer/projectm/releases) and unpack it.

#### Using a development version

If you prefer a bleeding-edge version or want to modify the code, clone the Git repository and initialize any
submodules:

```bash
sudo apt install git # Probably already installed
git clone https://github.com/projectM-visualizer/projectm.git /path/to/local/repo
cd /path/to/local/repo
git fetch --all --tags
git submodule init
git submodule update
```

### Build and install projectM

Older projectM releases use autoconf/automake for building. If your repository has a `CMakeLists.txt` file on the top
level, skip to the CMake part right below.

Replace `/usr/local` with your preferred installation prefix.

#### Configure the project using CMake

```bash
sudo apt install cmake
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
```

To generate Ninja scripts instead of Makefiles, add `-GNinja` to the above command.

#### Build and install

These commands will build projectM and install it to /usr/local or the configured installation prefix set in the step
before:

```bash
cmake --build . -- -j && sudo cmake --build . --target install 
```

**Note**: You won't need to use `sudo` if the install prefix is writeable by your non-privileged user.

#### Test projectM

If you have a desktop environment installed, you can now run `[prefix]/bin/projectMSDL`.

## Dependencies

Depending on the OS/distribution and packaging system, libraries might be split into separate packages with binaries and
development files. To build projectM, both binaries and development files need to be installed.

#### General build dependencies for all platforms:

* A working build toolchain (compiler, linker).
* CMake 3.21 or highter.
* **OpenGL**: 3D graphics library. Used to render the visualizations.
* **GLES3**: OpenGL libraries for embedded systems, version 3. Required to build projectM on mobile devices, Raspberry
  Pi and Emscripten.
* [**glm**](https://github.com/g-truc/glm):  OpenGL Mathematics library. It is optional, projectM will use a bundled
  version if not installed.
* [**SDL2**](https://github.com/libsdl-org/SDL): Simple Directmedia Layer. Version 2.0.5 or higher is required to build
  the development test application in the build directory.
* [**vcpkg**](https://github.com/microsoft/vcpkg): C++ Dependency Manager. Optional, but recommended to install
  library dependencies and/or using CMake to configure the build. Mainly used on Windows, but also works for Linux and
  macOS.

## Building on Linux and macOS

### Installing dependencies

- Linux distributions will have packages available for most (if not all) required libraries. The package names and
  commands to install them vary widely between distributions (and even versions of the same distribution). Please refer
  to the documentation of your build OS on how to find and install the required libraries.
- On *BSD, install the appropriate Ports with `pkg install`.
- On macOS, a working Xcode installation is required. CMake (and libSDL2 for the test UI) can be installed
  using [Homebrew](https://brew.sh/).

### Building

The steps documented below are a bare minimum quickstart guide on how to build and install the project. If you want to
configure the build to your needs, require more in-depth information about the build process and available tweaks, or on
how to use libprojectM in your own CMake-based projects, see [BUILDING-cmake.md](BUILDING-cmake.md).

Using CMake is the only supported way of building projectM since version 4.0. CMake is a platform-independent tool that
is able to generate files for multiple build systems and toolsets while using only a single set of build instructions.

Building the project with CMake requires two steps:

- Configure the build and generate project files.
- Build and install the project using the selected build tools.

**Note:** The build directory should always be different from the source directory. Generating the build files directly
inside the source tree is possible, but strongly discouraged. Using a subdirectory, e.g. `cmake-build` inside the source
directory is fine though.

This documentation only covers project-specific information. CMake is way too versatile and feature-rich to cover any
possible platform- and toolset-specific configuration details here. If you are not experienced in using CMake, please
first read the [official CMake documentation](https://cmake.org/cmake/help/latest/) (at least
the [User Interaction Guide](https://cmake.org/cmake/help/latest/guide/user-interaction/index.html)) for basic usage
instructions.

#### Configure the build

Configuring a non-debug build with default options and install prefix (`/usr/local`) can be done with these commands,
building in a subdirectory inside the source directory:

```shell
cd /path/to/source
mkdir cmake-build
cd cmake-build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

CMake will check all required dependencies and display any errors. If configuration was successful, a summary of the
build configuration is printed and CMake should display a `Generating done` line. The project is now ready to build.

#### Compile and install the project

Depending on your generator choice, you can use your selected toolset as usual to build and install projectM:

- With `Unix Makefiles`, run `make && sudo make install`.
- With `Ninja`, run `ninja && sudo ninja install`.
- With `Xcode`, select the appropriate target and configuration in Xcode and build it, or `INSTALL` to install the
  project.

You can also use CMake's build mode to run the selected toolset and build any specified target. CMake knows which
command to call and which parameters to pass, so the syntax works on all platforms with all generators. If you've
already set the top-level build directory as working directory, simply pass `.` as `/path/to/build/dir`:

```shell
cmake --build /path/to/build/dir --config Release
sudo cmake --build /path/to/build/dir --config Release --target install
```

If you don't need root permissions to install to the given path, running the second command without `sudo` is
sufficient.

To control how many files are compiled in parallel, pass the `--parallel N` argument to the build command, with `N`
being the number of concurrent compile commands. If not given explicitly, the build processor (make, ninja, MSBuild,
xcodebuild etc.) will decide the limit, which may range from one file at a time to all files at once.

If you want to provide arguments directly to the toolset command, add `--` at the end of the CMake command line followed
by any additional arguments. CMake will pass these *unchanged and unchecked* to the subcommand:

```shell
cmake --build /path/to/build/dir --config Release -- <additional build tool args>
```

## Building on Windows

To build the projectM library and the SDL-based standalone application, CMake must be used to create the project files
first. Using vcpkg to pull in the build dependencies is highly recommended, as CMake can't use NuGet (NuGet pulls in
dependencies using the project files, while CMake requires the libraries before creating the project files).

#### Installing the dependencies with vcpkg

As stated above, using vcpkg is the easiest way to get the required dependencies. First,
install [vcpkg from GitHub](https://github.com/microsoft/vcpkg) by following the official guide.

We've included a vcpkg manifest file in the repository root, designed to automatically install dependencies when you
create your solution.

#### Creating the Visual Studio solution

CMake provides separate generators for different Visual Studio versions. Newer CMake versions will support recent Visual
Studio releases, but may remove generators for older ones. To get a list of available generators from the command line,
use the `-G` switch without an argument. The CMake GUI will present you a dropdown list you can easily select from.

To set the build architecture in Visual Studio builds, use the `-A` switch and specify either `Win32` or `X64` as the
argument. If you want to build for both architectures, create separate build directories and configure them accordingly.

To make CMake aware of the installed vcpkg packages, simply use the provided toolchain file when configuring the
projectM build by
pointing [`CMAKE_TOOLCHAIN_FILE`](https://cmake.org/cmake/help/latest/variable/CMAKE_TOOLCHAIN_FILE.html) to it.

Here is a full command line example to create a Visual Studio 2022 solution for X64:

```commandline
cmake -G "Visual Studio 17 2022" -A "X64" -DCMAKE_TOOLCHAIN_FILE="<path to vcpkg>/scripts/buildsystems/vcpkg.cmake" -S "<path to source dir>" -B "<path to build dir>"
```

If you use the CMake GUI, check the "Specify toolchain file for cross-compiling" option in the first page of the
configuration assistant, then select the above `vcpkg.cmake` file on the second page.

Another option is to open the project folder in a recent Visual Studio version as a CMake project and configure CMake
using Visual Studio's JSON-based settings file.

#### Building the solution

To build the project, open the generated solution in Visual Studio and build it like any other solution. Each time the
CMake files are changed, Visual Studio will automatically regenerate the CMake build files and reload the solution
before continuing the build. Be aware that in old Visual Studio versions (2015 and earlier) the reload-and-continue
might not work properly.

You can also build the solution with msbuild via the command line, or use CMake's build wrapper to do that for you:

```commandline
cmake --build "<path to build dir>" --config Release
```

#### Using Ninja to build

The Ninja build system is shipped with Visual Studio since version 2019 and used by default if loading a CMake project
directly from within the IDE. Ninja can also be [installed separately](https://github.com/ninja-build/ninja/releases).

To configure the build directory for Ninja, pass `Ninja` or `Ninja Multi-Config` as the argument for the `-G` switch.
The difference between both generators is that the former uses `CMAKE_BUILD_TYPE` to specify the configuration (
e.g. `Debug` or `Release`) while the latter supports all configurations in a single build directory, specified during
build time.

The architecture is determined from the toolset, so make sure to run the commands in the correct Visual Studio command
prompt, e.g. "Native Tools for X64".

Configure and build for a single-configuration Release build with vcpkg:

```commandline
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="<path to vcpkg>/scripts/buildsystems/vcpkg.cmake" -S "<path to source dir>" -B "<path to build dir>"
cmake --build "<path to build dir>"
```

Same, but using the multi-configuration generator:

```commandline
cmake -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE="<path to vcpkg>/scripts/buildsystems/vcpkg.cmake" -S "<path to source dir>" -B "<path to build dir>"
cmake --build "<path to build dir>" --config Release
```

## Notes on other platforms and features

### Supported platforms

libprojectM is designed to be compatible with as many platforms and architectures as possible. Thus, the code base
doesn't use any non-portable code like OS-specific APIs or assembly instructions.

The code compiles for the following target architectures:

- i686 (x32 / x86)
- x86_64 (x64)
- armv7 (arm)
- aarch64 (arm64 / armv8)
- WASM

Other architectures such as PPC, RISC and MIPS should be supported as well. Big-endian support is untested and thus
might be buggy.

libprojectM builds at least on the following platforms:

- Windows
- Linux
- macOS (iOS/tvOS as well, but be aware that Apple's TOS may prevent its use in Store apps!)
- BSD derivates
- Android
- WebGL/WASM (using emscripten)

As long as a platform provides OpenGL Core 3.3 or OpenGL ES 3.2 libraries and supports at least C++ 14, libprojectM
should build fine.

### Raspberry Pi (and other embedded systems)

Make sure to enable GLES support by passing `-DENABLE_GLES=TRUE` to CMake's configuration command and install the
appropriate OS libraries. Otherwise, follow the Linux build instructions above.

For applications using projectM, it's highly recommended to keep certain performance-related settings low, such as:

- The per-point mesh resolution should be kept low, e.g. 48x32
- The rendering resolution should not exceed 720p on the Pi, though most presets will also run at ~60 FPS in 1080p

### Build using NDK for Android

To build projectM using the Android SDK, please refer to the official NDK docs:

> https://developer.android.com/ndk/guides/cmake

It is highly recommended using the latest NDK and CMake >= 3.21 for building.

### Using libprojectM with pkgconfig

Some UNIX build systems cannot use CMake config packages, like GNU autotools. To use libprojectM with such build
systems, projectM's build system also creates basic `.pc` files during the installation process.

Note that the resulting pkgconfig files will not necessarily work in all circumstances, because they are much less
flexible than CMake (or Meson). When using pkgconfig, some required libraries will probably not be linked
automatically (e.g. OpenGL libraries) and have to be added manually depending on the application needs.

### Frontends and audio capturing

Previous projectM versions (before 4.0) came with several UIs and audio capture implementations. To reduce the size of
the code base and separate the release cycles and development of the core library and the frontends, these have been
removed from libprojectM and either being rewritten or moved into their own, separate Git repository.

libprojectM on its own does not have any means of capturing audio, it simply takes PCM data via the API. Applications
using projectM can supply this data from their own audio sources, e.g. music playing in an audio player or capturing
audio data from OS or other external sources.
