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
option(ENABLE_PRESETS "Build and install bundled presets" ON)
option(ENABLE_NATIVE_PRESETS "Build and install native libraries written in C/C++" OFF)
option(ENABLE_TESTING "Build and install the projectM test suite" OFF)
option(ENABLE_EMSCRIPTEN "Build for web with emscripten" OFF)
cmake_dependent_option(ENABLE_SDL "Enable SDL2 support" ON "NOT ENABLE_EMSCRIPTEN;ENABLE_TESTING" ON)
cmake_dependent_option(ENABLE_GLES "Enable OpenGL ES support" OFF "NOT ENABLE_EMSCRIPTEN" ON)
cmake_dependent_option(ENABLE_THREADING "Enable multithreading support" ON "NOT ENABLE_EMSCRIPTEN;NOT CMAKE_SYSTEM_NAME STREQUAL Windows" OFF)
cmake_dependent_option(ENABLE_PULSEAUDIO "Build Pulseaudio-based Qt UI" OFF "ENABLE_QT;NOT ENABLE_EMSCRIPTEN;CMAKE_SYSTEM_NAME STREQUAL Linux" OFF)
cmake_dependent_option(ENABLE_JACK "Build JACK-based Qt and SDL UIs" OFF "ENABLE_QT;NOT ENABLE_EMSCRIPTEN;CMAKE_SYSTEM_NAME STREQUAL Linux" OFF)
cmake_dependent_option(ENABLE_LLVM "Enable LLVM JIT support" OFF "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_LIBVISUAL "Build and install the projectM libvisual plug-in" OFF "NOT ENABLE_EMSCRIPTEN;CMAKE_SYSTEM_NAME STREQUAL Linux" OFF)

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
    find_package(Qt5 REQUIRED COMPONENTS Gui OpenGL)
endif()

if(ENABLE_LIBVISUAL)
    find_package(libvisual REQUIRED)
    set(PROJECTM_LIBVISUAL_DIR "${LIBVISUAL_PLUGINSBASEDIR}/actor" CACHE STRING "Installation directory for the libvisual plug-in library.")
endif()

include(features.cmake)

add_subdirectory(presets)
add_subdirectory(src)

message(STATUS "")
message(STATUS "projectM v${PROJECT_VERSION}")
message(STATUS "    =====")
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
message(STATUS "    - - -")
message(STATUS "")
message(STATUS "    Applications:")
message(STATUS "    =====")
message(STATUS "")
message(STATUS "    libprojectM:             ON")
message(STATUS "    Presets:                 ${ENABLE_PRESETS}")
message(STATUS "    Native presets:          ${ENABLE_NATIVE_PRESETS}")
message(STATUS "    Tests:                   ${ENABLE_TESTING}")
message(STATUS "    Threading:               ${ENABLE_THREADING}")
message(STATUS "    SDL2:                    ${ENABLE_SDL}")
if(ENABLE_SDL)
    message(STATUS "    SDL2 version:            ${SDL2_VERSION}")
endif()
message(STATUS "    Pulseaudio UI:           ${ENABLE_PULSEAUDIO}")
if(ENABLE_PULSEAUDIO)
    message(STATUS "    Pulseaudio version:      ${PULSEAUDIO_VERSION}")
endif()
message(STATUS "    JACK UI:                 ${ENABLE_JACK}")
if(ENABLE_JACK)
    message(STATUS "    JACK version:            ${JACK_VERSION}")
endif()
if(ENABLE_PULSEAUDIO OR ENABLE_JACK)
    message(STATUS "    Qt version:              ${Qt5_VERSION}")
endif()
message(STATUS "    OpenGLES:                ${ENABLE_GLES}")
message(STATUS "    Emscripten:              ${ENABLE_EMSCRIPTEN}")
message(STATUS "    LLVM JIT:                ${ENABLE_LLVM}")
if(ENABLE_LLVM)
    message(STATUS "    LLVM version:            ${LLVM_VERSION}")
endif()
message(STATUS "    libvisual plug-in:       ${ENABLE_LIBVISUAL}")
message(STATUS "    Use system GLM:          ${USE_SYSTEM_GLM}")

message(AUTHOR_WARNING
        "The CMake build scripts for projectM are still in active development.\n"
        "This means that build parameters, exported target names and other things can change "
        "at any time until the new build system is officially documented and announced to be "
        "fully supported.\n"
        "DO NOT base any production work on it yet!\n"
        )

# Create CPack configuration
set(CPACK_PACKAGE_NAME "projectM")
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)
