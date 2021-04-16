cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(CMakeDependentOption)
include(CheckSymbolExists)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_POSITION_INDEPENDENT_CODE YES)

option(ENABLE_DEBUG_POSTFIX "Add \"d\" after library names for debug builds" ON)
if(ENABLE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX d)
endif()

# The API (SO) and detailed library versions for the shared library.
set(PROJECTM_SO_VERSION "3")
set(PROJECTM_LIB_VERSION "3.1.1")

project(projectm
        LANGUAGES C CXX
        VERSION 1.1.0
        )

if(NOT MSVC)
    include(EnableCFlagsIfSupported)
    enable_cflags_if_supported(
            -Wall
            -Wchar-subscripts
            -Wformat-security
            -Wpointer-arith
            -Wshadow
            -Wsign-compare
            -Wtype-limits
    )
endif()

set(PROJECTM_BIN_DIR "bin" CACHE STRING "Executable installation directory, relative to the install prefix.")
set(PROJECTM_DATADIR_PATH "share/projectM" CACHE STRING "Default (absolute) path to the projectM data files (presets etc.)")
set(PROJECTM_LIB_DIR "lib" CACHE STRING "Library installation directory, relative to the install prefix.")
set(PROJECTM_INCLUDE_DIR "include" CACHE STRING "Header installation directory, relative to the install prefix.")

# Feature options, including dependencies.
option(ENABLE_PRESETS "Build and install bundled presets" ON)
option(ENABLE_TESTING "Build and install the projectM test suite" OFF)
option(ENABLE_EMSCRIPTEN "Build for web with emscripten" OFF)
cmake_dependent_option(ENABLE_SDL "Enable SDL2 support" ON "NOT ENABLE_EMSCRIPTEN;ENABLE_TESTING" ON)
cmake_dependent_option(ENABLE_GLES "Enable OpenGL ES support" OFF "NOT ENABLE_EMSCRIPTEN" ON)
cmake_dependent_option(ENABLE_THREADING "Enable multithreading support" ON "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_QT "Enable Qt5. Required for Pulseaudio and JACK support." OFF "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_PULSEAUDIO "Enable Pulseaudio support" OFF "ENABLE_QT;NOT ENABLE_EMSCRIPTEN;CMAKE_SYSTEM_NAME STREQUAL Linux" OFF)
cmake_dependent_option(ENABLE_JACK "Enable JACK support" OFF "ENABLE_QT;NOT ENABLE_EMSCRIPTEN;CMAKE_SYSTEM_NAME STREQUAL Linux" OFF)
cmake_dependent_option(ENABLE_LLVM "Enable LLVM JIT support" OFF "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_LIBVISUAL "Build and install the projectM libvisual plug-in" OFF "NOT ENABLE_EMSCRIPTEN;CMAKE_SYSTEM_NAME STREQUAL Linux" OFF)

option(USE_SYSTEM_GLM OFF "Use system/external GLM library instead of the bundled one")

if(NOT USE_SYSTEM_GLM)
    # Pre-instantiate some templates for commonly used specializations.
    add_library(glm_headers INTERFACE)
    target_include_directories(glm_headers INTERFACE "${CMAKE_SOURCE_DIR}/vendor")
    add_library(GLM::GLM ALIAS glm_headers)
    message(STATUS "Using bundled GLM library")
else()
    find_package(GLM REQUIRED)
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
endif()

if(ENABLE_GLES)
    find_package(GLES3 REQUIRED)
else()
    find_package(OpenGL REQUIRED)
endif()

if(ENABLE_QT)
    find_package(Qt5 REQUIRED COMPONENTS Gui OpenGL)
endif()

if(ENABLE_LLVM)
    find_package(LLVM REQUIRED)
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

if(ENABLE_LIBVISUAL)
    find_package(libvisual REQUIRED)
    set(PROJECTM_LIBVISUAL_DIR "${LIBVISUAL_PLUGINSBASEDIR}/actor" CACHE STRING "Installation directory for the libvisual plug-in library.")
endif()

# Create global configuration header
configure_file(config.h.in "${CMAKE_BINARY_DIR}/include/config.h")
include_directories("${CMAKE_BINARY_DIR}/include")
add_compile_definitions(HAVE_CONFIG_H)

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
message(STATUS "    presets:                 ${ENABLE_PRESETS}")
message(STATUS "    tests:                   ${ENABLE_TESTING}")
message(STATUS "    Threading:               ${ENABLE_THREADING}")
message(STATUS "    SDL:                     ${ENABLE_SDL}")
message(STATUS "    Qt:                      ${ENABLE_QT}")
message(STATUS "    Pulseaudio:              ${ENABLE_PULSEAUDIO}")
message(STATUS "    JACK:                    ${ENABLE_JACK}")
message(STATUS "    OpenGLES:                ${ENABLE_GLES}")
message(STATUS "    Emscripten:              ${ENABLE_EMSCRIPTEN}")
message(STATUS "    LLVM JIT:                ${ENABLE_LLVM}")
message(STATUS "    libvisual plug-in:       ${ENABLE_LIBVISUAL}")
message(STATUS "    USE_SYSTEM_GLM:          ${USE_SYSTEM_GLM}")
