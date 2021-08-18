cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

if(CMAKE_VERSION VERSION_LESS 3.19 AND CMAKE_GENERATOR STREQUAL "Xcode")
    message(AUTHOR_WARNING "Using a CMake version before 3.19 with a recent Xcode SDK and the Xcode generator "
            "will likely result in CMake failing to find the AppleClang compiler. Either upgrade CMake to at least "
            "version 3.19 or use a different generator, e.g. \"Unix Makefiles\" or \"Ninja\".")
endif()

include(CMakeDependentOption)
include(CheckSymbolExists)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_POSITION_INDEPENDENT_CODE YES)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(ENABLE_DEBUG_POSTFIX "Add \"d\" after library names for debug builds" ON)
if(ENABLE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX d)
endif()

# The API (SO) and detailed library versions for the shared library.
set(PROJECTM_SO_VERSION "3")
set(PROJECTM_LIB_VERSION "3.1.1")

project(projectm
        LANGUAGES C CXX
        VERSION 3.1.13
        )

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(PROJECTM_BIN_DIR "bin" CACHE STRING "Executable installation directory, relative to the install prefix.")
set(PROJECTM_DATADIR_PATH "share/projectM" CACHE STRING "Default (absolute) path to the projectM data files (presets etc.)")
set(PROJECTM_LIB_DIR "lib" CACHE STRING "Library installation directory, relative to the install prefix.")
set(PROJECTM_INCLUDE_DIR "include" CACHE STRING "Header installation directory, relative to the install prefix.")

# Feature options, including dependencies.
option(ENABLE_STATIC_LIB "Build and install libprojectM as a static library" ON)
cmake_dependent_option(ENABLE_SHARED_LIB "Build and install libprojectM as a shared library" ON "NOT CMAKE_SYSTEM_NAME STREQUAL Windows" OFF)
cmake_dependent_option(ENABLE_SHARED_LINKING "Link all built UI applications against the shared library." OFF "ENABLE_SHARED_LIB" OFF)
option(ENABLE_DOXYGEN "Build and install Doxygen source code documentation in PROJECTM_DATADIR_PATH/docs." OFF)
option(ENABLE_CXX_INTERFACE "Enable exporting all C++ symbols, not only the C API, in the shared library. Warning: This is dangerous!" OFF)
option(ENABLE_PRESETS "Build and install bundled presets" ON)
option(ENABLE_NATIVE_PRESETS "Build and install native libraries written in C/C++" OFF)
option(ENABLE_TESTING "Build and install the projectM test suite" OFF)
option(ENABLE_EMSCRIPTEN "Build for web with emscripten" OFF)
cmake_dependent_option(ENABLE_SDL "Enable SDL2 support" ON "NOT ENABLE_EMSCRIPTEN;NOT ENABLE_TESTING" ON)
cmake_dependent_option(ENABLE_SDL_UI "Build the SDL2-based reference UI" ON "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_GLES "Enable OpenGL ES support" OFF "NOT ENABLE_EMSCRIPTEN" ON)
cmake_dependent_option(ENABLE_THREADING "Enable multithreading support" ON "NOT ENABLE_EMSCRIPTEN;NOT CMAKE_SYSTEM_NAME STREQUAL Windows" OFF)
cmake_dependent_option(ENABLE_PULSEAUDIO "Build PulseAudio-based Qt UI" OFF "NOT ENABLE_EMSCRIPTEN;CMAKE_SYSTEM_NAME STREQUAL Linux" OFF)
cmake_dependent_option(ENABLE_JACK "Build JACK-based Qt and SDL UIs" OFF "NOT ENABLE_EMSCRIPTEN;CMAKE_SYSTEM_NAME STREQUAL Linux" OFF)
cmake_dependent_option(ENABLE_QT "Build Qt UI library" OFF "NOT ENABLE_PULSEAUDIO;NOT ENABLE_JACK" ON)
cmake_dependent_option(ENABLE_LLVM "Enable LLVM JIT support" OFF "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_LIBVISUAL "Build and install the projectM libvisual plug-in" OFF "NOT ENABLE_EMSCRIPTEN;CMAKE_SYSTEM_NAME STREQUAL Linux" OFF)

if(NOT ENABLE_STATIC_LIB AND NOT ENABLE_SHARED_LIB)
    message(FATAL_ERROR "At least one of either ENABLE_STATIC_LIB or ENABLE_SHARED_LIB options must be set to ON.")
endif()

if(ENABLE_DOXYGEN)
    find_package(Doxygen REQUIRED)
endif()

find_package(GLM)
if(NOT TARGET GLM::GLM)
    add_library(GLM::GLM INTERFACE IMPORTED)
    set_target_properties(GLM::GLM PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/vendor"
            )

    message(STATUS "GLM library not found, using bundled version.")
    set(USE_SYSTEM_GLM OFF)
else()
    set(USE_SYSTEM_GLM ON)
endif()

if(ENABLE_EMSCRIPTEN)
    message(STATUS "${CMAKE_C_COMPILER}")
    check_symbol_exists(__EMSCRIPTEN__ "" HAVE_EMSCRIPTEN)
    if(NOT HAVE_EMSCRIPTEN)
        message(FATAL_ERROR "You are not using an emscripten compiler.")
    endif()
endif()

if(ENABLE_SDL)
    find_package(SDL2 REQUIRED)

    # Apply some fixes, as SDL2's CMake support is new and still a WiP.
    include(SDL2Target)
endif()

if(ENABLE_GLES)
    # Might be implemented in the CMake OpenGL module.
    # We may provide a find module in the future, and add support for CMake's own module later:
    # https://gitlab.kitware.com/cmake/cmake/-/blob/3fc3b43933d0b486aa4eb5d63fc257475feff348/Modules/FindOpenGL.cmake#L520
    message(FATAL_ERROR "GLES support is currently not implemented for CMake builds.")
    find_package(GLES3 REQUIRED)
else()
    find_package(OpenGL REQUIRED)
    set(PROJECTM_OPENGL_LIBRARIES OpenGL::GL)
    # GLX is required by SOIL2 on platforms with the X Window System (e.g. most Linux distributions)
    if(TARGET OpenGL::GLX)
        list(APPEND PROJECTM_OPENGL_LIBRARIES OpenGL::GLX)
    endif()
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        find_package(GLEW REQUIRED)
        list(APPEND PROJECTM_OPENGL_LIBRARIES GLEW::glew)
    endif()
endif()

if(ENABLE_THREADING)
    set(CMAKE_THREAD_PREFER_PTHREAD YES)
    find_package(Threads REQUIRED)
    if(NOT CMAKE_USE_PTHREADS_INIT)
        message(FATAL_ERROR "Threading support requested - pthread support is required, but not available.")
    endif()
    set(USE_THREADS YES)
endif()

if(ENABLE_LLVM)
    find_package(LLVM REQUIRED)
    if(LLVM_VERSION VERSION_LESS 10.0)
        message(FATAL_ERROR "LLVM JIT support requires at least version 10.0, but only ${LLVM_VERSION} was found.")
    endif()
    set(HAVE_LLVM TRUE)
else()
    unset(HAVE_LLVM)
endif()

if(ENABLE_PULSEAUDIO)
    find_package(Pulseaudio REQUIRED)
endif()

if(ENABLE_JACK)
    find_package(JACK REQUIRED)
endif()

if(ENABLE_PULSEAUDIO OR ENABLE_JACK)
    set(QT_REQUIRED_COMPONENTS Gui Widgets OpenGL Xml)
    if(DEFINED QT_VERSION)
        find_package(Qt${QT_VERSION} REQUIRED COMPONENTS ${QT_REQUIRED_COMPONENTS})
    else()
        # Try to determine the Qt version available, starting with Qt6
        message(STATUS "Determining Qt version. To use a specific version, set QT_VERSION to either 5 or 6.")
        find_package(Qt6 QUIET COMPONENTS ${QT_REQUIRED_COMPONENTS})
        if(TARGET Qt6::Core)
            set(QT_VERSION 6)
        else()
            find_package(Qt5 QUIET COMPONENTS ${QT_REQUIRED_COMPONENTS})
            if(TARGET Qt5::Core)
                set(QT_VERSION 5)
            else()
                message(FATAL_ERROR "Could not find either Qt 5 or 6, one is required to build the PulseAudio or JACK UIs.")
            endif()
        endif()
    endif()

    # OpenGLWidgets were put into their own component since Qt 6.
    if(QT_VERSION EQUAL 6)
        list(APPEND QT_REQUIRED_COMPONENTS OpenGLWidgets)
        find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
    endif()

    set(QT_LINK_TARGETS "")
    foreach(component ${QT_REQUIRED_COMPONENTS})
        list(APPEND QT_LINK_TARGETS Qt${QT_VERSION}::${component})
    endforeach()
endif()

if(ENABLE_LIBVISUAL)
    find_package(libvisual REQUIRED)
    set(PROJECTM_LIBVISUAL_DIR "${LIBVISUAL_PLUGINSBASEDIR}/actor" CACHE STRING "Installation directory for the libvisual plug-in library.")
endif()

if(ENABLE_CXX_INTERFACE)
    set(CMAKE_C_VISIBILITY_PRESET default)
    set(CMAKE_CXX_VISIBILITY_PRESET default)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN OFF)
else()
    set(CMAKE_C_VISIBILITY_PRESET hidden)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
endif()

include(features.cmake)

add_subdirectory(presets)
add_subdirectory(src)

message(STATUS "")
message(STATUS "projectM v${PROJECT_VERSION}")
message(STATUS "==============================================")
message(STATUS "")
message(STATUS "    prefix:                  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "    libdir:                  ${PROJECTM_LIB_DIR}")
message(STATUS "    includedir:              ${PROJECTM_INCLUDE_DIR}")
message(STATUS "    bindir:                  ${PROJECTM_BIN_DIR}")
message(STATUS "    libvisual plugin dir:    ${PROJECTM_LIBVISUAL_DIR}")
message(STATUS "")
message(STATUS "    compiler:                ${CMAKE_CXX_COMPILER}")
message(STATUS "    cflags:                  ${CMAKE_C_FLAGS}")
message(STATUS "    cxxflags:                ${CMAKE_CXX_FLAGS}")
message(STATUS "    ldflags:                 ${CMAKE_SHARED_LINKER_FLAGS}")
message(STATUS "")
message(STATUS "    DATADIR_PATH:            ${PROJECTM_DATADIR_PATH}")
message(STATUS "")
message(STATUS "Features:")
message(STATUS "==============================================")
message(STATUS "")
message(STATUS "    Presets:                 ${ENABLE_PRESETS}")
message(STATUS "    Native presets:          ${ENABLE_NATIVE_PRESETS}")
message(STATUS "    Threading:               ${ENABLE_THREADING}")
message(STATUS "    SDL2:                    ${ENABLE_SDL}")
if(ENABLE_SDL)
    message(STATUS "        SDL2 version:        ${SDL2_VERSION}")
endif()
message(STATUS "    OpenGLES:                ${ENABLE_GLES}")
message(STATUS "    Emscripten:              ${ENABLE_EMSCRIPTEN}")
message(STATUS "    LLVM JIT:                ${ENABLE_LLVM}")
if(ENABLE_LLVM)
    message(STATUS "        LLVM version:        ${LLVM_VERSION}")
endif()
message(STATUS "    Use system GLM:          ${USE_SYSTEM_GLM}")
message(STATUS "    Link UI with shared lib: ${ENABLE_SHARED_LINKING}")
message(STATUS "")
message(STATUS "Targets and applications:")
message(STATUS "==============================================")
message(STATUS "")
message(STATUS "    libprojectM static:      ${ENABLE_STATIC_LIB}")
message(STATUS "    libprojectM shared:      ${ENABLE_SHARED_LIB}")
message(STATUS "    SDL2 UI:                 ${ENABLE_SDL_UI}")
message(STATUS "    Doxygen API docs:        ${ENABLE_DOXYGEN}")
message(STATUS "    Tests:                   ${ENABLE_TESTING}")
message(STATUS "    PulseAudio UI:           ${ENABLE_PULSEAUDIO}")
if(ENABLE_PULSEAUDIO)
    message(STATUS "        PulseAudio version:  ${PULSEAUDIO_VERSION}")
endif()
message(STATUS "    JACK UI:                 ${ENABLE_JACK}")
if(ENABLE_JACK)
    message(STATUS "        JACK version:        ${JACK_VERSION}")
endif()
if(ENABLE_PULSEAUDIO OR ENABLE_JACK)
    message(STATUS "    Qt version:              ${Qt${QT_VERSION}_VERSION}")
endif()
message(STATUS "    libvisual plug-in:       ${ENABLE_LIBVISUAL}")
message(STATUS "")

message(AUTHOR_WARNING
        "The CMake build scripts for projectM are still in active development.\n"
        "This means that build parameters, exported target names and other things can change "
        "at any time until the new build system is officially documented and announced to be "
        "fully supported.\n"
        "DO NOT base any production work on it yet!\n"
        )

if(ENABLE_CXX_INTERFACE)
    message(AUTHOR_WARNING "This build is configured to export all C++ symbols in the shared library.\n"
            "Using C++ STL types across library borders only works if all components were built "
            "with the exact same toolchain and C++ language level, otherwise it will cause crashes.\n"
            "Enabling this option will not export additional symbols in Windows DLL builds.\n"
            "Only use this if you know what you're doing. You have been warned!"
            )
endif()

# Create CPack configuration
set(CPACK_PACKAGE_NAME "projectM")
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)
