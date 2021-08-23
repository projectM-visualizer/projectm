# Building with CMake

This file contains in-depth information for building with the CMake build system.

## Quickstart

Want to build it fast?

Required tools: `cmake`. 

```shell
mkdir build
cd build
cmake ..
make -j8
```

Output binaries will be in `build/src/...`.

SDL2 binary can be found in `build/src/projectM-sdl/projectMSDL`.


## Selecting a specific project file generator

To specify a CMake generator, use the `-G` switch, followed by the generator name. Some newer generators take an
additional architecture using the `-A` switch. To list all available generators available on your current platform,
leave out the generator name:

```shell
cmake -G
```

Additional information on the supported generators can be
found [in the CMake documentation](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html).

### Popular generators

By default, CMake will use the [`Unix Makefiles`](https://cmake.org/cmake/help/latest/generator/Unix%20Makefiles.html)
generator on Linux and macOS, which is a good choice and should work. Yet in some circumstances, you might want to
generate project files for a specific build tool or IDE:

```shell
cmake -G "Unix Makefiles" -S /path/to/source/dir -B /path/to/build/dir
```

A common alternative is the [`Ninja`](https://cmake.org/cmake/help/latest/generator/Ninja.html) generator, which
requires `ninja` to be installed. It is mostly a `make`
replacement with less overhead and should work equally well. It is supported on all major platforms, including Windows:

```shell
cmake -G Ninja -S /path/to/source/dir -B /path/to/build/dir
```

On macOS, CMake also supports the [`Xcode`](https://cmake.org/cmake/help/latest/generator/Xcode.html) generator. It will
create an `.xcodeproj` bundle which you can open in Xcode. It also adds support for automatic code signing, which might
be required if your application using projectM needs to be notarized for store deployment.

```shell
cmake -G Xcode -S /path/to/source/dir -B /path/to/build/dir
```

If you develop on Windows, you will possibly use Visual Studio. While recent visual Studio versions have CMake support
built-in, you can still pre-generate the solution and project files and open the `.sln` file from the build directory.
CMake provides a separate generator for each Visual Studio release. For Visual Studio 2019 you would use
the [`Visual Studio 16 2019`](https://cmake.org/cmake/help/latest/generator/Visual%20Studio%2016%202019.html) generator
and provide an additional architecture parameter:

```shell
cmake -G "Visual Studio 16 2019" -A "X64" -S /path/to/source/dir -B /path/to/build/dir
```

It is not possible to generate multi-arch solutions with CMake though. You need to create separate build directories and
use the respective `-A` switch for each.

## Project-specific configuration options

CMake has no built-in way of printing all available configuration options. You can either refer to the
top-level `CMakeLists.txt` which contains a block of `option` and `cmake_dependent_option` commands, or use one of the
available CMake UIs which will display the options after configuring the project once.

### Boolean switches

The following table also gives you an overview of the available options and their defaults. All options accept a boolean
value (`YES`/`NO`, `TRUE`/`FALSE`, `ON`/`OFF` or `1`/`0`) and can be provided on the configuration-phase command line
using the `-D` switch.

| CMake option            | Default | Required dependencies | Description                                                                                                |
|-------------------------|---------|-----------------------|------------------------------------------------------------------------------------------------------------|
| `ENABLE_DEBUG_PREFIX`   | `ON`    |                       | Adds `d` to the name of any binary file in debug builds.                                                   |
| `ENABLE_EMSCRIPTEN`     | `OFF`   | `GLES`, `Emscripten`  | Build for the web using Emscripten.                                                                        |
| `ENABLE_SDL`            | `ON`    | `SDL2`                | Builds and installs the standalone SDL visualizer application. Also required for Emscripten and the tests. |
| `ENABLE_GLES`           | `OFF`   | `GLES`                | Use OpenGL ES 3 profile for rendering instead of the Core profile.                                         |
| `ENABLE_THREADING`      | `ON`    | `pthreads`            | Enable multithreading support. If enabled, will cause an error if pthreads are not available.              |
| `ENABLE_NATIVE_PRESETS` | `OFF`   |                       | Builds and installs the binary (native) preset libraries.                                                  |
| `ENABLE_PRESETS`        | `ON`    |                       | Installs several thousand shipped presets.                                                                 |
| `ENABLE_PULSEAUDIO`     | `OFF`   | `Qt5`, `Pulseaudio`   | Build the Qt5-based Pulseaudio visualizer application.                                                     |
| `ENABLE_JACK`           | `OFF`   | `Qt5`, `JACK`         | Build the Qt5-based JACK visualizer application.                                                           |
| `ENABLE_LIBVISUAL`      | `OFF`   | `libvisual-0.4`       | Build the libvisual plug-in/actor library.                                                                 |
| `ENABLE_TESTING`        | `OFF`   | `SDL2`                | Builds the unit tests.                                                                                     |
| `ENABLE_LLVM`           | `OFF`   | `LLVM`                | Enables experimental LLVM JIT support.                                                                     |

### Path options

There are also a few textual parameters that can be used to fine-tune the installation directories. Relative paths in
the following options are appended to the value
of [`CMAKE_INSTALL_PREFIX`](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html) (which, on most UNIX
platforms, defaults to `/usr/local`):

| CMake option            | Default          | Description                                                                      |
|-------------------------|------------------|----------------------------------------------------------------------------------|
| `PROJECTM_BIN_DIR`      | `bin`            | Directory where executables (e.g. the SDL standalone application) are installed. |
| `PROJECTM_LIB_DIR`      | `lib`            | Directory where libprojectM is installed[<sup>[*]</sup>](#libvisual-path).       |
| `PROJECTM_INCLUDE_DIR`  | `include`        | Directory where the libprojectM include files will be installed under.           |
| `PROJECTM_DATADIR_PATH` | `share/projectM` | Directory where the default configuration and presets are installed under.       |

<a name="libvisual-path"></a>[*]: The libvisual projectM plug-in install location is determined automatically via
pkgconfig and not influenced by this option.

## Always perform out-of-tree builds!

Most classic IDEs and build systems directly make use of the source tree and create project files, temporary build
artifacts (e.g. object files) and the final binaries in the same directory structure as the source files. An advantage
of this approach is that you can find all compiled binaries side-by-side with their sources and generated headers are
already in the same directories as the source files including them. This approach has some drawbacks though:

- Only a single build configuration is supported as files are overwritten in-place.
- A lot of noise is created in the source directory, making it hard to distinguish between generated and original source
  files.
- A very large `.gitignore` file is required to cover all unwanted files.
- Mistakes in the build scripts can overwrite source files, causing errors and destroy uncommitted work.

Some of these can be mitigated by providing additional targets (`make clean` and `make distclean`) or creating
subdirectories for Debug/Release build configurations.

While CMake also supports in-tree builds, it is "discouraged" in the official documentation, for the above reasons.
Building out-of-tree allows it to create multiple build directories with different configurations which do not influence
each other in any way. If a build directory contains unwanted artifacts, and you want to start fresh, simply delete and
recreate the whole directory - no work is lost.

This project follow this principle by treating the original source tree as read-only and avoiding potential conflicts:

- Everything under [`CMAKE_SOURCE_DIR`](https://cmake.org/cmake/help/latest/variable/CMAKE_SOURCE_DIR.html) must only be
  read, never changed or written to.
- Everything under [`CMAKE_BINARY_DIR`](https://cmake.org/cmake/help/latest/variable/CMAKE_BINARY_DIR.html) is temporary
  and related to the current build configuration.
- When generating configuration-dependent files,
  use [`CMAKE_CONFIGURATION_TYPES`](https://cmake.org/cmake/help/latest/variable/CMAKE_CONFIGURATION_TYPES.html)
  and [`CMAKE_BUILD_TYPE`](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html) to create non-conflicting
  files in the build tree.

While this project will not force you to build out-of-tree, there is no mechanism to clean up the generated files after
running cmake in-tree.

## CMake build directory layout

If you are new to CMake, the way of how CMake creates the build directory and where it creates the build targets might
be confusing. Here is a summary of what's in the build directory and how it is structured in general.

### Using files from the build tree

It is generally not good practice to directly take binaries and other files from the build tree for packaging, for
several reasons:

1. The directory structure is generated by CMake and depends on the generator used. The layout might change between
   CMake versions, even for the same generator.
2. On platforms with RPATH support, CMake will store absolute paths in executables and shared libraries which point to
   the absolute paths of any linked dependencies, either from the build tree or external libraries as well. These
   binaries are not relocatable and will most certainly not work if run on any other computer (or even on the same after
   deleting the build directory).
3. For some configurations, even Release build artifacts may contain debug symbols until they are installed.

It is fine to build and run executables from the build directory for development and debugging. For packaging, always
use the `install` target and copy files from there.

### Generated files

In the top-level build directory, CMake creates a few files that are present on any platform:

- `CMakeCache.txt`: This file contains all variables and build settings CMake needs to remember from the first
  configuration run. This file can be edited on demand either manually or using a CMake UI to change any values. On the
  next build, CMake will regenerate the project files if this file has been modified.
- `cmake_install.cmake`: Contains generated install-related settings.
- `install_manifest.txt`: After installing the project, this file contains a list with absolute filenames of all
  installed files. It can be used for packaging or deleting installed files as CMake doesn't define an `uninstall`
  target.
- The top-level project file for use with the selected build toolset, e.g. `Makefile`, `build.ninja`, `projectm.sln`
  or `projectm.xcodeproj`, plus additional toolset-specific files.

The projectM build files generate additional files used in the build and install phases:

- `config.inp`: The default configuration file, by default installed to `<prefix>/share/projectM`.
- `libprojectM.pc`: Package configuration file for use with `pkgconfig`.
- `include/config.h`: A forced include file that contains macro definitions to enable or disable certain code features
  based on the build configuration and availability of platform headers and features. Similar to the
  autoheader-generated file.

### Subdirectory structure

The rest of the directory structure generally resembles the source tree. Source directories containing
a `CMakeLists.txt` file will also be created in the build tree with the same relative path. Each of these subdirectories
contains a `CMakeFiles` directory with CMake-internal data, generated project files for the select toolset, e.g.
makefiles and any temporary compile artifacts.

### Executable and library locations

Build targets - shared/static libraries and executables - are created in the same subdirectory in the build tree as
the `CMakeLists.txt` file that defines the target in the source tree (which, in most cases, resides in the same
directory as the source files). Depending on the generator used, the binaries are created directly in the directory for
single-configuration generators (like `Unix Makefiles` or `Ninja`) and in a subdirectory with the configuration name,
e.g. `Debug` or `Release`, for multi-configuration generators like `Xcode` or `Visual Studio 16 2019`.

You may also find additional files and symbolic links in the same location depending on the platform, e.g. `.pdb` files
on Windows.

## Using libprojectM in other CMake projects

The projectM library can be used as a static library and, on non-Windows platforms, as a shared library in other
CMake-based projects to provide embedded audio visualization. There are two ways:

- Importing the library CMake targets directly from the build tree without installation.
- Using the library from an installed location or package with the provided CMake package config files.

### Importing libprojectM targets from the build tree

This approach is useful for projects that either require more in-depth access to the projectM library files, especially
to headers that are not installed as part of the public API. This might cause issues if the internal headers change, but
gives a broader set of features and more control to the developer.

To use the targets, follow these steps:

- Configure the build directory as needed.
- Build the required library targets `projectM_static` and `projectM_shared` as needed, or simply everything.
- Include the file `src/libprojectM/projectM-exports.cmake` from the projectM build tree in your project.

All targets and their interface properties are now defined and can be used.

#### Example

```cmake
# libprojectM.a/.lib is already built.
set(PROJECTM_BUILD_DIR "/some/path/to/projectM/build")
include("${PROJECTM_BUILD_DIR}/src/libprojectM/projectM-exports.cmake")

add_executable(MyApp main.cpp)

target_link_libraries(MyApp PRIVATE libprojectM::static)
```

This will also add all projectM include directories to the target's source files, pointing to the respective projectM
source directories.

Look at the generated file in the build tree if you need more details about the available targets.

### Importing libprojectM targets from an installed version

This is the recommended way of importing libprojectM in your project. This project installs a set of CMake files
in `<PREFIX>/<LIBDIR>/cmake/libprojectM`, containing target definitions, version and dependency checks as well as any
additional libraries required for linking. Other projects then use CMake's `find_package` command to search for these
files in [different locations](https://cmake.org/cmake/help/latest/command/find_package.html#search-procedure).

In the case projectM libraries and headers are not installed in any system search path, you need to add either the
install prefix path (the top-level install dir) or the directory containing the libraries (the `lib` dir by default) to
the [`CMAKE_PREFIX_PATH`](https://cmake.org/cmake/help/latest/variable/CMAKE_PREFIX_PATH.html) list.

If the package was found, you can then link against one of these provided targets:

- `libprojectM::static` - The static library (`.a` or `.lib`).
- `libprojectM::shared` - The shared library (`.so`, `.dylib` or `.dll`).

Note that the availability of the targets depend on the platform and build configuration, so only one of those might be
available. You can check with `if(TARGET libprojectM::static)` and `if(TARGET libprojectM::shared)` to select the
available or preferred one.

Depending on how the package was built, targets might be available for multiple configurations or only `Release`. CMake
will automatically select the most appropriate one to link.

Include dirs, additional link dependencies and possible compiler options will be propagated to any target the library is
linked to.

#### Example

Links the shared library preferably, with fallback to static:

```cmake
find_package(libprojectM REQUIRED)

if(TARGET libprojectM::shared)
    set(PROJECTM_LINK_TARGET libprojectM::shared)
else()
    # If this target is also unavailable, CMake will error out on target_link_libraries().
    set(PROJECTM_LINK_TARGET libprojectM::static)
endif()

add_executable(MyApp main.cpp)

target_link_libraries(MyApp PRIVATE ${PROJECTM_LINK_TARGET})
```
