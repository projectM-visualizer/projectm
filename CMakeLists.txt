cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

include(CMakeDependentOption)
include(CheckSymbolExists)

# Save the current source/binary dirs if we're in a subdirectory of a larger CMake project.
set(PROJECTM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(PROJECTM_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_POSITION_INDEPENDENT_CODE YES)

# Don't export any symbols except those explicitly exported.
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(ENABLE_DEBUG_POSTFIX "Add \"d\" (by default) after library names for debug builds." ON)
if(ENABLE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Output file debug postfix. Default is \"d\".")
endif()

project(libprojectM
        LANGUAGES C CXX
        VERSION 4.0.0
        )

# The API (SO) version for the shared library. Should be incremented whenever the binary interface changes
# in a non-backwards-compatible way, e.g. changing parameters or return values of existing functions or removing
# functions. Adding new function should be okay if documented.
set(PROJECTM_SO_VERSION "4")

# Base filename of all installed libraries. Also used as package name in pkgconfig.
set(PROJECTM_LIBRARY_BASE_OUTPUT_NAME "projectM-${PROJECT_VERSION_MAJOR}")

# The actual (full) library version of projectM
set(PROJECTM_LIB_VERSION "${CMAKE_PROJECT_VERSION}")

list(APPEND CMAKE_MODULE_PATH "${PROJECTM_SOURCE_DIR}/cmake")

include(VCSVersion)
include(GNUInstallDirs)

set(PROJECTM_BIN_DIR "${CMAKE_INSTALL_BINDIR}" CACHE STRING "Executable installation directory, relative to the install prefix.")
set(PROJECTM_LIB_DIR "${CMAKE_INSTALL_LIBDIR}" CACHE STRING "Library installation directory, relative to the install prefix.")
set(PROJECTM_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}" CACHE STRING "Header installation directory, relative to the install prefix.")

# Dummy file for merged static libs.
set(PROJECTM_DUMMY_SOURCE_FILE "${PROJECTM_BINARY_DIR}/dummy.cpp")
file(TOUCH "${PROJECTM_DUMMY_SOURCE_FILE}")

if(CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(ENABLE_EMSCRIPTEN ON CACHE BOOL "Build for web with emscripten. Will also build the SDL2-based entrypoint." FORCE)
else()
    set(ENABLE_EMSCRIPTEN OFF CACHE BOOL "Build for web with emscripten. Requires emscripten toolset for building." FORCE)
endif()

# Feature options, including dependencies.
option(BUILD_TESTING "Build the libprojectM test suite" OFF)
cmake_dependent_option(BUILD_SHARED_LIBS "Build and install libprojectM as a shared libraries. If OFF, builds as static libraries." ON "NOT ENABLE_EMSCRIPTEN" OFF)
option(ENABLE_PLAYLIST "Enable building the playlist management library" ON)
cmake_dependent_option(ENABLE_SDL_UI "Build the SDL2-based developer test UI" OFF "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_GLES "Enable OpenGL ES support" ON "NOT ENABLE_EMSCRIPTEN AND NOT CMAKE_SYSTEM_NAME STREQUAL Android" ON)
cmake_dependent_option(ENABLE_OPENMP "Enable OpenMP support if available" ON "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_THREADING "Enable multithreading support." ON "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_INSTALL "Enable installing projectM libraries and headers." OFF "NOT PROJECT_IS_TOP_LEVEL" ON)
option(ENABLE_SYSTEM_GLM "Enable use of system-install GLM library" OFF)

# Experimental/unsupported features
cmake_dependent_option(ENABLE_LLVM "Enable experimental LLVM JIT support" OFF "NOT ENABLE_EMSCRIPTEN" OFF)
option(ENABLE_CXX_INTERFACE "Enable exporting C++ symbols for ProjectM and PCM classes, not only the C API. Warning: This is not very portable." OFF)

if(ENABLE_SYSTEM_GLM)
    find_package(GLM REQUIRED)
else()
    add_library(GLM::GLM INTERFACE IMPORTED)
    set_target_properties(GLM::GLM PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${PROJECTM_SOURCE_DIR}/vendor"
            )
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
            "SHELL:-s NO_DISABLE_EXCEPTION_CATCHING"
            )

    add_link_options(
            "SHELL:-s USE_SDL=2"
            "SHELL:-s MIN_WEBGL_VERSION=2"
            "SHELL:-s MAX_WEBGL_VERSION=2"
            "SHELL:-s FULL_ES2=1"
            "SHELL:-s ALLOW_MEMORY_GROWTH=1"
            "SHELL:-s NO_DISABLE_EXCEPTION_CATCHING"
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
        message(STATUS "Building for OpenGL Embedded Profile")
        if(NOT CMAKE_SYSTEM_NAME STREQUAL Linux
                AND NOT CMAKE_SYSTEM_NAME STREQUAL Android
                AND NOT CMAKE_SYSTEM_NAME STREQUAL iOS)
            message(FATAL_ERROR "OpenGL ES 3 support is currently only available for Linux and iOS platforms. You're building for ${CMAKE_SYSTEM_NAME}.")
        endif()

        # We use a local find script for OpenGL::GLES3 until the proposed changes are merged upstream.
        if(CMAKE_SYSTEM_NAME STREQUAL iOS)
            # Do nothing, link directly to OpenGLES.framework later
        else()
            list(APPEND CMAKE_MODULE_PATH "${PROJECTM_SOURCE_DIR}/cmake/gles")
            find_package(OpenGL REQUIRED COMPONENTS GLES3)
            if(NOT TARGET OpenGL::GLES3)
                message(FATAL_ERROR "No suitable GLES3 library was found.")
            endif()
            set(PROJECTM_OPENGL_LIBRARIES OpenGL::GLES3)
        endif()

        set(USE_GLES ON)
    else()
        message(STATUS "Building for OpenGL Core Profile")
        find_package(OpenGL REQUIRED)
        set(PROJECTM_OPENGL_LIBRARIES OpenGL::GL)
        # GLX is required by SOIL2 on platforms with the X Window System (e.g. most Linux distributions)
        if(TARGET OpenGL::GLX)
            list(APPEND PROJECTM_OPENGL_LIBRARIES OpenGL::GLX)
        endif()
        if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
            find_package(GLEW REQUIRED)
            # Prefer shared, but check for static lib if shared is not available.
            if(TARGET GLEW::glew)
                list(APPEND PROJECTM_OPENGL_LIBRARIES GLEW::glew)
            elseif(TARGET GLEW::glew_s)
                list(APPEND PROJECTM_OPENGL_LIBRARIES GLEW::glew_s)
            endif()
        endif()
    endif()

    if(ENABLE_OPENMP)
        find_package(OpenMP)
        if(NOT OpenMP_FOUND OR NOT OpenMP_CXX_FOUND)
            set(ENABLE_OPENMP OFF)
        endif()
    endif()

    if(ENABLE_THREADING)
        find_package(Threads REQUIRED)
        set(PROJECTM_USE_THREADS YES)
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
add_subdirectory(vendor)
add_subdirectory(src)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

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
message(STATUS "    Build shared libraries:  ${BUILD_SHARED_LIBS}")
message(STATUS "    Threading:               ${ENABLE_THREADING}")
message(STATUS "    SDL2:                    ${ENABLE_SDL_UI}")
if(ENABLE_SDL_UI)
    message(STATUS "        SDL2 version:        ${SDL2_VERSION}")
endif()
message(STATUS "    OpenGL ES:               ${ENABLE_GLES}")
message(STATUS "    OpenMP:                  ${ENABLE_OPENMP}")
if(ENABLE_OPENMP)
    message(STATUS "        OpenMP version:      ${OpenMP_CXX_VERSION}")
endif()
message(STATUS "    Emscripten:              ${ENABLE_EMSCRIPTEN}")
message(STATUS "    LLVM JIT:                ${ENABLE_LLVM}")
if(ENABLE_LLVM)
    message(STATUS "        LLVM version:        ${LLVM_VERSION}")
endif()
message(STATUS "    Use system GLM:          ${ENABLE_SYSTEM_GLM}")
message(STATUS "    Link UI with shared lib: ${ENABLE_SHARED_LINKING}")
message(STATUS "")
message(STATUS "Targets and applications:")
message(STATUS "==============================================")
message(STATUS "")
message(STATUS "    libprojectM:             (always built)")
message(STATUS "    Playlist library:        ${ENABLE_PLAYLIST}")
message(STATUS "    SDL2 Test UI:            ${ENABLE_SDL_UI}")
message(STATUS "    Tests:                   ${BUILD_TESTING}")
message(STATUS "")

if(ENABLE_CXX_INTERFACE)
    message(AUTHOR_WARNING
            "This build is configured to export C++ symbols for ProjectM and PCM classes in the shared library.\n"
            "Using C++ STL types across library borders only works if all components were built "
            "with the exact same toolchain and C++ language level, otherwise it will cause crashes.\n"
            "Only use this if you know what you're doing. You have been warned!"
            )
endif()

# Create CPack configuration
set(CPACK_PACKAGE_NAME "projectM")
set(CPACK_VERBATIM_VARIABLES YES)
include(CPack)
