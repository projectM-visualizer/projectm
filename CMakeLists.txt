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

# General build options
option(ENABLE_SYSTEM_GLM "Enable use of system-install GLM library" OFF)
option(ENABLE_SYSTEM_PROJECTM_EVAL "Enable use of a system-installed/external projectM-eval library" ON)
option(ENABLE_DEBUG_POSTFIX "Add \"d\" (by default) after library names for debug builds." ON)
option(ENABLE_PLAYLIST "Enable building the playlist management library" ON)
option(ENABLE_BOOST_FILESYSTEM "Force the use of boost::filesystem, even if the compiler supports C++17." OFF)
option(ENABLE_SDL_UI "Build the SDL2-based developer test UI. Ignored when building with Emscripten or for Android." OFF)
option(ENABLE_VERBOSE_LOGGING "Enables TRACE and DEBUG logging even in release builds, negatively affecting the performance." OFF)

option(BUILD_TESTING "Build the libprojectM test suite" OFF)
option(BUILD_DOCS "Build documentation" OFF)

# Enable vcpkg manifest features according to the build options set
if(ENABLE_SYSTEM_GLM)
    list(APPEND VCPKG_MANIFEST_FEATURES external-glm)
endif()
if(ENABLE_SYSTEM_PROJECTM_EVAL)
    list(APPEND VCPKG_MANIFEST_FEATURES external-evallib)
endif()
if(ENABLE_BOOST_FILESYSTEM)
    list(APPEND VCPKG_MANIFEST_FEATURES boost-filesystem)
endif()
if(ENABLE_SDL_UI)
    list(APPEND VCPKG_MANIFEST_FEATURES gui)
endif()
if(BUILD_TESTING)
    list(APPEND VCPKG_MANIFEST_FEATURES test)
endif()

if(ENABLE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "Output file debug postfix. Default is \"d\".")
endif()

project(libprojectM
        LANGUAGES C CXX
        VERSION 4.1.0
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

if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(PROJECTM_RUNTIME_DIR "${PROJECTM_BIN_DIR}")
else()
    set(PROJECTM_RUNTIME_DIR "${PROJECTM_LIB_DIR}")
endif()

# Dummy file for merged static libs.
set(PROJECTM_DUMMY_SOURCE_FILE "${PROJECTM_BINARY_DIR}/dummy.cpp")
file(TOUCH "${PROJECTM_DUMMY_SOURCE_FILE}")

if(CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    set(ENABLE_EMSCRIPTEN ON CACHE BOOL "Build for web with emscripten. Will also build the SDL2-based entrypoint." FORCE)
    option(USE_PTHREADS "Enable multithreading support" OFF)
else()
    set(ENABLE_EMSCRIPTEN OFF CACHE BOOL "Build for web with emscripten. Requires emscripten toolset for building." FORCE)
endif()

# Compiler-/system-dependent options, including dependencies.
cmake_dependent_option(BUILD_SHARED_LIBS "Build and install libprojectM as a shared libraries. If OFF, builds as static libraries." ON "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_GLES "Enable OpenGL ES support" OFF "NOT ENABLE_EMSCRIPTEN AND NOT CMAKE_SYSTEM_NAME STREQUAL Android" ON)
cmake_dependent_option(ENABLE_INSTALL "Enable installing projectM libraries and headers." OFF "NOT PROJECT_IS_TOP_LEVEL" ON)

# Experimental/unsupported features
option(ENABLE_CXX_INTERFACE "Enable exporting C++ symbols for ProjectM and PCM classes, not only the C API. Warning: This is not very portable." OFF)

if(ENABLE_SYSTEM_GLM)
    find_package(GLM REQUIRED)
endif()

if(ENABLE_SYSTEM_PROJECTM_EVAL)
    find_package(projectM-Eval QUIET)
    if(NOT TARGET projectM::Eval)
        message(STATUS "projectM-Eval could not be found externally. Using sources from vendor dir (if present).")
    else()
        message(STATUS "Found external projectM-Eval: Version ${projectM-Eval_VERSION}")
    endif()
endif()

if(NOT BUILD_SHARED_LIBS AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Add "lib" in front of static library files to allow installing both shared and static libs in the same dir.
    set(CMAKE_STATIC_LIBRARY_PREFIX lib)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL Darwin)
    # Silence OpenGL API deprecation warnings on macOS.
    add_compile_definitions(GL_SILENCE_DEPRECATION)
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
            "SHELL:-s NO_DISABLE_EXCEPTION_CATCHING"
            )

    add_link_options(
            "SHELL:-s USE_SDL=2"
            "SHELL:-s MIN_WEBGL_VERSION=2"
            "SHELL:-s MAX_WEBGL_VERSION=2"
            "SHELL:-s FULL_ES2=1"
            "SHELL:-s FULL_ES3=1"
            "SHELL:-s ALLOW_MEMORY_GROWTH=1"
            "SHELL:-s NO_DISABLE_EXCEPTION_CATCHING"
            )

    if(USE_PTHREADS)
        add_compile_options("SHELL:-s USE_PTHREADS=1")
        add_link_options("SHELL:-s USE_PTHREADS=1")
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
        set(USE_GLES ON)
    else()
        message(STATUS "Building for OpenGL Core Profile")
        if(WIN32)
            find_package(OpenGL REQUIRED)
            set(PROJECTM_OPENGL_LIBRARIES OpenGL::GL)
        endif()
    endif()
endif()

# Disable trace/debug logging in release builds unless explicitly requested
add_compile_definitions(
        $<$<OR:$<CONFIG:Debug>,$<BOOL:${ENABLE_VERBOSE_LOGGING}>>:ENABLE_DEBUG_LOGGING>
        )

if(BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    find_package(Sphinx REQUIRED breathe exhale)
    set(DOXYGEN_GENERATE_HTML NO)
    set(DOXYGEN_GENERATE_XML YES)
    # All doxygen comments are in header files. Processing cpp files
    # produces duplicate C++ function definitions in doxygen, resulting
    # in various problems.
    # See https://github.com/breathe-doc/breathe/issues/772
    set(DOXYGEN_EXCLUDE_PATTERNS "*.cpp")

    doxygen_add_docs(
            projectm_doxygen
            src
            COMMENT "Generate HTML documentation")

    sphinx_add_docs(
            projectm_sphinx
            BREATHE_PROJECTS projectm_doxygen
            BUILDER html
            SOURCE_DIRECTORY docs)
endif()

add_subdirectory(vendor)

include(features.cmake)

add_subdirectory(presets)
add_subdirectory(src)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

message(STATUS "")
message(STATUS "libprojectM v${PROJECT_VERSION}")
message(STATUS "==============================================")
message(STATUS "")
message(STATUS "    prefix:                      ${CMAKE_INSTALL_PREFIX}")
message(STATUS "    libdir:                      ${PROJECTM_LIB_DIR}")
message(STATUS "    includedir:                  ${PROJECTM_INCLUDE_DIR}")
message(STATUS "    bindir:                      ${PROJECTM_BIN_DIR}")
message(STATUS "")
message(STATUS "    compiler:                    ${CMAKE_CXX_COMPILER}")
message(STATUS "    cflags:                      ${CMAKE_C_FLAGS}")
message(STATUS "    cxxflags:                    ${CMAKE_CXX_FLAGS}")
message(STATUS "    ldflags:                     ${CMAKE_SHARED_LINKER_FLAGS}")
message(STATUS "")
message(STATUS "Features:")
message(STATUS "==============================================")
message(STATUS "")
message(STATUS "    Build shared libraries:      ${BUILD_SHARED_LIBS}")
if(ENABLE_BOOST_FILESYSTEM)
    message(STATUS "    Filesystem support:          Boost")
    message(STATUS "        Boost version:           ${Boost_VERSION}")
else()
    message(STATUS "    Filesystem support:          C++17 STL")
endif()
message(STATUS "    SDL2:                        ${ENABLE_SDL_UI}")
if(ENABLE_SDL_UI)
    message(STATUS "        SDL2 version:            ${SDL2_VERSION}")
endif()
message(STATUS "    OpenGL ES:                   ${ENABLE_GLES}")
message(STATUS "    Emscripten:                  ${ENABLE_EMSCRIPTEN}")
if(CMAKE_SYSTEM_NAME STREQUAL Emscripten)
    message(STATUS "    - PThreads:              ${USE_PTHREADS}")
endif()
message(STATUS "    Use system GLM:              ${ENABLE_SYSTEM_GLM}")
message(STATUS "    Use system projectM-eval:    ${ENABLE_SYSTEM_PROJECTM_EVAL}")
if(ENABLE_SYSTEM_PROJECTM_EVAL)
    message(STATUS "        projectM-eval version:       ${projectM-Eval_VERSION}")
endif()
message(STATUS "    Link UI with shared lib:     ${ENABLE_SHARED_LINKING}")
message(STATUS "")
message(STATUS "Targets and applications:")
message(STATUS "==============================================")
message(STATUS "")
message(STATUS "    libprojectM:                 (always built)")
message(STATUS "    Playlist library:            ${ENABLE_PLAYLIST}")
message(STATUS "    SDL2 Test UI:                ${ENABLE_SDL_UI}")
message(STATUS "    Tests:                       ${BUILD_TESTING}")
message(STATUS "    Documentation:               ${BUILD_DOCS}")
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
