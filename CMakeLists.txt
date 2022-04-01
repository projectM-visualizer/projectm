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
set(PROJECTM_SO_VERSION "4")
set(PROJECTM_LIB_VERSION "4.0.0")

project(libprojectM
        LANGUAGES C CXX
        VERSION 3.99
        )

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(PROJECTM_BIN_DIR "bin" CACHE STRING "Executable installation directory, relative to the install prefix.")
set(PROJECTM_LIB_DIR "lib" CACHE STRING "Library installation directory, relative to the install prefix.")
set(PROJECTM_INCLUDE_DIR "include" CACHE STRING "Header installation directory, relative to the install prefix.")

if(CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(ENABLE_EMSCRIPTEN ON CACHE BOOL "Build for web with emscripten. Will also build the SDL2-based entrypoint." FORCE)
else()
    set(ENABLE_EMSCRIPTEN OFF CACHE BOOL "Build for web with emscripten. Requires emscripten toolset for building." FORCE)
endif()

# Feature options, including dependencies.
option(ENABLE_STATIC_LIB "Build and install libprojectM as a static library" ON)
cmake_dependent_option(ENABLE_SHARED_LIB "Build and install libprojectM as a shared library" ON "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_SHARED_LINKING "Link the SDL test UI against the shared library." OFF "ENABLE_SHARED_LIB" OFF)
option(ENABLE_DOXYGEN "Build and install Doxygen source code documentation in PROJECTM_DATADIR_PATH/docs." OFF)
option(ENABLE_CXX_INTERFACE "Enable exporting all C++ symbols, not only the C API, in the shared library. Warning: This is not very portable." OFF)
option(ENABLE_PRESETS "Build and install bundled presets" ON)
option(ENABLE_NATIVE_PRESETS "Build and install native preset loading support and preset libraries written in C/C++" OFF)
option(ENABLE_TESTING "Build the libprojectM test suite" OFF)
cmake_dependent_option(ENABLE_SDL_UI "Build the SDL2-based developer test UI" OFF "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_GLES "Enable OpenGL ES support" OFF "NOT ENABLE_EMSCRIPTEN" ON)
option(ENABLE_THREADING "Enable multithreading support. Use with care with emscripten." ON)
cmake_dependent_option(ENABLE_LLVM "Enable LLVM JIT support" OFF "NOT ENABLE_EMSCRIPTEN" OFF)

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
    message(STATUS "${CMAKE_C_COMPILER} on ${CMAKE_SYSTEM_NAME}")
    check_symbol_exists(__EMSCRIPTEN__ "" HAVE_EMSCRIPTEN)
    if(NOT HAVE_EMSCRIPTEN)
        message(FATAL_ERROR "You are not using an emscripten compiler.")
    endif()

    # emscripten uses different options to compile and link libraries, so we can't use find_package().
    # Instead, specifying the required options directly to emcc is the way to go.
    # Note: The "SHELL:" syntax is required to pass each argument as-is, but without quotes and CMake's de-duplication.
    add_compile_options(
        "SHELL:-s USE_SDL=2"
        "SHELL:-s MIN_WEBGL_VERSION=2"
        "SHELL:-s MAX_WEBGL_VERSION=2"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        )

    add_link_options(
        "SHELL:-s USE_SDL=2"
        "SHELL:-s MIN_WEBGL_VERSION=2"
        "SHELL:-s MAX_WEBGL_VERSION=2"
        "SHELL:-s FULL_ES2=1"
        "SHELL:-s ALLOW_MEMORY_GROWTH=1"
        )

    if(ENABLE_THREADING)
        message(AUTHOR_WARNING "Threading on emscripten is deemed stable, but may have issues. Use with care.\n"
                "See https://emscripten.org/docs/porting/pthreads.html for more information.")
        add_compile_options(-pthread)
        add_link_options(-pthread)
    endif()

    set(USE_GLES ON)
else()
    if(ENABLE_SDL_UI)
        find_package(SDL2 REQUIRED)

        # Apply some fixes, as SDL2's CMake support is new and still a WiP.
        include(SDL2Target)
    endif()

    if(ENABLE_GLES)
        if(NOT CMAKE_SYSTEM_NAME STREQUAL Linux)
            message(FATAL_ERROR "OpenGL ES 3 support is currently only available for Linux platforms.")
        endif()

        # We use a local find script for OpenGL::GLES3 until the proposed changes are merged upstream.
        list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/gles")
        find_package(OpenGL REQUIRED COMPONENTS GLES3)
        if(NOT TARGET OpenGL::GLES3)
            message(FATAL_ERROR "No suitable GLES3 library was found.")
        endif()

        set(PROJECTM_OPENGL_LIBRARIES OpenGL::GLES3)
        set(USE_GLES ON)
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
        find_package(Threads REQUIRED)
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
add_subdirectory(tests)

message(STATUS "")
message(STATUS "libprojectM v${PROJECT_VERSION}")
message(STATUS "==============================================")
message(STATUS "")
message(STATUS "    prefix:                  ${CMAKE_INSTALL_PREFIX}")
message(STATUS "    libdir:                  ${PROJECTM_LIB_DIR}")
message(STATUS "    includedir:              ${PROJECTM_INCLUDE_DIR}")
message(STATUS "    bindir:                  ${PROJECTM_BIN_DIR}")
message(STATUS "")
message(STATUS "    compiler:                ${CMAKE_CXX_COMPILER}")
message(STATUS "    cflags:                  ${CMAKE_C_FLAGS}")
message(STATUS "    cxxflags:                ${CMAKE_CXX_FLAGS}")
message(STATUS "    ldflags:                 ${CMAKE_SHARED_LINKER_FLAGS}")
message(STATUS "")
message(STATUS "Features:")
message(STATUS "==============================================")
message(STATUS "")
message(STATUS "    Presets:                 ${ENABLE_PRESETS}")
message(STATUS "    Native presets:          ${ENABLE_NATIVE_PRESETS}")
message(STATUS "    Threading:               ${ENABLE_THREADING}")
message(STATUS "    SDL2:                    ${ENABLE_SDL_UI}")
if(ENABLE_SDL)
    message(STATUS "        SDL2 version:        ${SDL2_VERSION}")
endif()
message(STATUS "    OpenGL ES:               ${ENABLE_GLES}")
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
message(STATUS "    SDL2 Test UI:            ${ENABLE_SDL_UI}")
message(STATUS "    Doxygen API docs:        ${ENABLE_DOXYGEN}")
message(STATUS "    Tests:                   ${ENABLE_TESTING}")
message(STATUS "")

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
