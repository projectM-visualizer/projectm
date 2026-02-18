Building projectM from source
=============================

Suggested: use CMake. See `BUILDING-cmake.md <./BUILDING-cmake.md>`__.

This document describes the deprecated GNU Autotools setup.

Quick Start (Debian / Ubuntu)
-----------------------------

For other operating systems (Windows/macOS), see the OS-specific
sections below.

Install the build tools and dependencies
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Mandatory packages:

.. code:: bash

   sudo apt install build-essential libgl1-mesa-dev mesa-common-dev libsdl2-dev libglm-dev

Optional packages for additional features:

.. code:: bash

   sudo apt install qtbase5-dev # For building Qt-based UIs
   sudo apt install llvm-dev # for using the experimental LLVM Jit
   sudo apt install libvisual-0.4-dev # To build the libvisual plug-in
   sudo apt install libjack-jackd2-dev # To build the JACK visualizer application
   sudo apt install ninja # To build projectM with Ninja instead of make

Download the projectM sources
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you want to use a stable version of projectM, download the latest
release from the `Releases page on
GitHub <https://github.com/projectM-visualizer/projectm/releases>`__ and
unpack it. You can then skip to the next step.

If you prefer a bleeding-edge version or want to modify the code, clone
the Git repository and initialize any submodules:

.. code:: bash

   sudo apt install git # Probably already installed
   git clone https://github.com/projectM-visualizer/projectm.git /path/to/local/repo
   cd /path/to/local/repo
   git fetch --all --tags
   git submodule init
   git submodule update

Build and install projectM
~~~~~~~~~~~~~~~~~~~~~~~~~~

Older projectM releases use autoconf/automake for building. If your
repository has a ``CMakeLists.txt`` file on the top level, skip to the
CMake part right below.

Replace ``/usr/local`` with your preferred installation prefix.

Configure the project using CMake
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: bash

   sudo apt install cmake
   mkdir build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..

To generate Ninja scripts instead of Makefiles, add ``-GNinja`` to the
above command.

Build and install
^^^^^^^^^^^^^^^^^

These commands will build projectM and install it to /usr/local or the
configured installation prefix set in the step before:

.. code:: bash

   cmake --build . -- -j && sudo cmake --build . --target install 

**Note**: You won’t need to use ``sudo`` if the install prefix is
writeable by your non-privileged user.

Test projectM
^^^^^^^^^^^^^

If you have a desktop environment installed, you can now run
``[prefix]/bin/projectMSDL``.

Dependencies
------------

Depending on the OS/distribution and packaging system, libraries might
be split into separate packages with binaries and development files. To
build projectM, both binaries and development files need to be
installed.

General build dependencies for all platforms:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  A working build toolchain.
-  **OpenGL**: 3D graphics library. Used to render the visualizations.
-  **GLES3**: OpenGL libraries for embedded systems, version 3. Required
   to build projectM on mobile devices, Raspberry Pi and Emscripten.
-  `glm <https://github.com/g-truc/glm>`__: OpenGL Mathematics library.
   Optional, will use a bundled version with autotools or if not
   installed.
-  `SDL2 <https://github.com/libsdl-org/SDL>`__: Simple Directmedia
   Layer. Version 2.0.5 or higher is required to build the standalone
   visualizer application (projectMSDL).

Only relevant for Linux distributions, FreeBSD and macOS:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

-  `Qt5 <https://www.qt.io/>`__: Qt cross-platform UI framework. Used to
   build the Pulseaudio and JACK visualizer applications. Requires the
   ``Gui`` and ``OpenGL`` component libraries/frameworks.
-  `Pulseaudio <https://www.freedesktop.org/wiki/Software/PulseAudio/>`__:
   Sound system for POSIX platforms. Required to build the Pulseaudio
   visualizer application.
-  `JACK <https://jackaudio.org/>`__: Real-time audio server. Required
   to build the JACK visualizer application.
-  `libvisual 0.4 <http://libvisual.org/>`__: Audio visualization
   library with plug-in support. Required to build the projectM
   libvisual plug-in.
-  `CMake <https://cmake.org/>`__: Used to generate platform-specific
   build files.

Only relevant for Windows:
~~~~~~~~~~~~~~~~~~~~~~~~~~

-  `vcpkg <https://github.com/microsoft/vcpkg>`__: C++ Library Manager
   for Windows. Optional, but recommended to install the aforementioned
   library dependencies and/or using CMake to configure the build.
-  `NuGet <https://www.nuget.org/>`__: Dependency manager for .NET.
   Required to build the EyeTune app.

Building on Linux and macOS
---------------------------

Installing dependencies
~~~~~~~~~~~~~~~~~~~~~~~

-  Linux distributions will have packages available for most (if not
   all) required libraries. The package names and commands to install
   them vary widely between distributions (and even versions of the same
   distribution). Please refer to the documentation of your build OS on
   how to find and install the required libraries.
-  On \*BSD, install the appropriate Ports with ``pkg install``.
-  On macOS, using `Homebrew <https://brew.sh/>`__ is the recommended
   way of installing any dependencies not supplied by Xcode.

Building with CMake
~~~~~~~~~~~~~~~~~~~

--------------

:exclamation: **IMPORTANT NOTE**: Currently, CMake build support is
still in active development and considered unfinished. It is working and
produces running binaries, but there are still some features, build
internals and whole targets missing. While testing the CMake build files
on any platform and feedback on this is strongly encouraged, CMake-based
builds should not yet be used in any production environment until this
message is gone.

--------------

The steps documented below are a bare minimum quickstart guide on how to
build and install the project. If you want to configure the build to
your needs, require more in-depth information about the build process
and available tweaks, or on how to use libprojectM in your own
CMake-based projects, see `BUILDING-cmake.md <BUILDING-cmake.md>`__.

Using CMake is the recommended and future-proof way of building
projectM. CMake is a platform-independent tool that is able to generate
files for multiple build systems and toolsets while using only a single
set of build instructions. CMake support is still new and in
development, but will replace the other project files (automake/autoconf
scripts, Visual Studio solutions and Xcode projects) in this repository
once mature and stable.

Building the project with CMake requires two steps:

-  Configure the build and generate project files.
-  Build and install the project using the selected build tools.

**Note:** When building with CMake, the build directory should always be
separate from the source directory. Generating the build files directly
inside the source tree is possible, but strongly discouraged. Using a
subdirectory, e.g. ``cmake-build`` inside the source directory is fine
though.

This documentation only covers project-specific information. CMake is
way too versatile and feature-rich to cover any possible platform- and
toolset-specific configuration details here. If you are not experienced
in using CMake, please first read the `official CMake
documentation <https://cmake.org/cmake/help/latest/>`__ (at least the
`User Interaction
Guide <https://cmake.org/cmake/help/latest/guide/user-interaction/index.html>`__)
for basic usage instructions.

Configure the build
^^^^^^^^^^^^^^^^^^^

Configuring a non-debug build with default options and install prefix
(``/usr/local``) can be done with these commands, building in a
subdirectory inside the source directory:

.. code:: shell

   cd /path/to/source
   mkdir cmake-build
   cd cmake-build
   cmake -DCMAKE_BUILD_TYPE=Release ..

CMake will check all required dependencies and display any errors. If
configuration was successful, a summary of the build configuration is
printed and CMake should display a ``Generating done`` line. The project
is now ready to build.

Compile and install the project
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Depending on your generator choice, you can use your selected toolset as
usual to build and install projectM:

-  With ``Unix Makefiles``, run ``make && sudo make install``.
-  With ``Ninja``, run ``ninja && sudo ninja install``.
-  With ``Xcode``, select the appropriate target and configuration in
   Xcode and build it, or ``INSTALL`` to install the project.

You can also use CMake’s build mode to run the selected toolset and
build any specified target. CMake knows which command to call and which
parameters to pass, so the syntax works on all platforms with all
generators. If you’ve already set the top-level build directory as
working directory, simply pass ``.`` as ``/path/to/build/dir``:

.. code:: shell

   cmake --build /path/to/build/dir --config Release
   sudo cmake --build /path/to/build/dir --config Release --target install

If you don’t need root permissions to install running the second command
without ``sudo`` is sufficient.

If you want to provide arguments directly to the toolset command, add
``--`` at the end of the CMake command line followed by any additional
arguments. CMake will pass these *unchanged and unchecked* to the
subcommand:

.. code:: shell

   cmake --build /path/to/build/dir --config Release -- -j 4

Building on Windows
-------------------

To build the projectM library and the SDL-based standalone application,
CMake must be used to create the project files first. Using vcpkg to
pull in the build dependencies is highly recommended, as CMake can’t use
NuGet (NuGet pulls in dependencies using the project files, while CMake
requires the libraries before creating the project files).

Installing the dependencies with vcpkg
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As stated above, using vcpkg is the easiest way to get the required
dependencies. First, install `vcpkg from
GitHub <https://github.com/microsoft/vcpkg>`__ by following the official
guide.

We’ve included a vcpkg manifest file in the repository root, designed to
automatically install dependencies when you create your solution.

Creating the Visual Studio solution
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

CMake provides separate generators for different Visual Studio versions.
Newer CMake versions will support recent Visual Studio releases, but may
remove generators for older ones. To get a list of available generators
from the command line, use the ``-G`` switch without an argument. The
CMake GUI will present you a dropdown list you can easily select from.

To set the build architecture in Visual Studio builds, use the ``-A``
switch and specify either ``Win32`` or ``X64`` as the argument. If you
want to build for both architectures, create separate build directories
and configure them accordingly.

To make CMake aware of the installed vcpkg packages, simply use the
provided toolchain file when configuring the projectM build by pointing
```CMAKE_TOOLCHAIN_FILE`` <https://cmake.org/cmake/help/latest/variable/CMAKE_TOOLCHAIN_FILE.html>`__
to it.

Here is a full command line example to create a Visual Studio 2019
solution for X64:

.. code:: commandline

   cmake -G "Visual Studio 16 2019" -A "X64" -DCMAKE_TOOLCHAIN_FILE="<path to vcpkg>/scripts/buildsystems/vcpkg.cmake" -S "<path to source dir>" -B "<path to build dir>"

If you use the CMake GUI, check the “Specify toolchain file for
cross-compiling” option in the first page of the configuration
assistant, then select the above ``vcpkg.cmake`` file on the second
page.

Another option is to open the project folder in a recent Visual Studio
version as a CMake project and configure CMake using Visual Studio’s
JSON-based settings file.

Building the solution
^^^^^^^^^^^^^^^^^^^^^

To build the project, open the generated solution in Visual Studio and
build it like any other solution. Each time the CMake files are changed,
Visual Studio will automatically regenerate the CMake build files and
reload the solution before continuing the build. Be aware that in old
Visual Studio versions (2015 and earlier) the reload-and-continue might
not work properly.

You can also build the solution with msbuild via the command line, or
use CMake’s build wrapper to do that for you:

.. code:: commandline

   cmake --build "<path to build dir>" --config Release

Using Ninja to build
^^^^^^^^^^^^^^^^^^^^

The Ninja build system is shipped with Visual Studio since version 2019
and used by default if loading a CMake project directly from within the
IDE. Ninja can also be `installed
separately <https://github.com/ninja-build/ninja/releases>`__.

To configure the build directory for Ninja, pass ``Ninja`` or
``Ninja Multi-Config`` as the argument for the ``-G`` switch. The
difference between both generators is that the former uses
``CMAKE_BUILD_TYPE`` to specify the configuration ( e.g. ``Debug`` or
``Release``) while the latter supports all configurations in a single
build directory, specified during build time.

The architecture is determined from the toolset, so make sure to run the
commands in the correct Visual Studio command prompt, e.g. “Native Tools
for X64”.

Configure and build for a single-configuration Release build with vcpkg:

.. code:: commandline

   cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="<path to vcpkg>/scripts/buildsystems/vcpkg.cmake" -S "<path to source dir>" -B "<path to build dir>"
   cmake --build "<path to build dir>"

Same, but using the multi-configuration generator:

.. code:: commandline

   cmake -G "Ninja Multi-Config" -DCMAKE_TOOLCHAIN_FILE="<path to vcpkg>/scripts/buildsystems/vcpkg.cmake" -S "<path to source dir>" -B "<path to build dir>"
   cmake --build "<path to build dir>" --config Release

Notes on other platforms and features
-------------------------------------

Raspberry Pi (and other embedded systems)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  projectM is arch-independent, although there are some SSE2
   enhancements for x86
-  `Notes on running on raspberry
   pi <https://github.com/projectM-visualizer/projectm/issues/115>`__

Build using NDK for Android
~~~~~~~~~~~~~~~~~~~~~~~~~~~

To build projectM using the Android SDK, please refer to the official
NDK docs:

   https://developer.android.com/ndk/guides/cmake

It is highly recommended using the latest NDK and CMake >= 3.21 for
building.

libprojectM
-----------

``libprojectM`` is the core library. It is made up of three
sub-libraries:

Renderer
^^^^^^^^

Made up of everything in ``src/libprojectM/Renderer``. These files
compose the ``libRenderer`` sub-library.

Assets
~~~~~~

``libprojectM`` can either have a configuration hard-coded or load from
a configuration file. It’s up to each application to decide how to load
the config file. The config file can have paths defined specifying where
to load fonts and presets from.

You will want to install the config file and presets somewhere, and then
define that path for the application you’re trying to build.
