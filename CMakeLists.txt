cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(CMakeDependentOption)
include(CheckSymbolExists)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

project(projectm
        LANGUAGES C CXX
        VERSION 1.1.0
        )

# Feature options, including dependencies.
option(ENABLE_EMSCRIPTEN "Build for web with emscripten" OFF)
cmake_dependent_option(ENABLE_SDL "Enable SDL2 support" ON "NOT ENABLE_EMSCRIPTEN" ON)
cmake_dependent_option(ENABLE_GLES "Enable OpenGL ES support" OFF "NOT ENABLE_EMSCRIPTEN" ON)
cmake_dependent_option(ENABLE_THREADING "Enable multithreading support" ON "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_QT "Enable Qt5. Required for Pulseaudio and JACK support." OFF "NOT ENABLE_EMSCRIPTEN" OFF)
cmake_dependent_option(ENABLE_PULSEAUDIO "Enable Pulseaudio support" OFF "ENABLE_QT;NOT ENABLE_EMSCRIPTEN;CMAKE_SYSTEM_NAME STREQUAL Linux" OFF)
cmake_dependent_option(ENABLE_JACK "Enable JACK support" OFF "ENABLE_QT;NOT ENABLE_EMSCRIPTEN;CMAKE_SYSTEM_NAME STREQUAL Linux" OFF)
cmake_dependent_option(ENABLE_LLVM "Enable LLVM JIT support" OFF "NOT ENABLE_EMSCRIPTEN" OFF)

option(USE_SYSTEM_GLM OFF "Use system/external GLM library instead of the bundled one")

if(NOT USE_SYSTEM_GLM)
    # Pre-instantiate some templates for commonly used specializations.
    set(GLM_STATIC_LIBRARY_ENABLE ON)
    add_subdirectory(vendor/glm)
    add_library(GLM::GLM ALIAS glm_static)
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
endif()

if(ENABLE_PULSEAUDIO)
    find_package(Pulseaudio REQUIRED)
endif()

if(ENABLE_JACK)
    find_package(JACK REQUIRED)
endif()

message(STATUS "projectM build configuration:")
message(STATUS "    ENABLE_QT: ${ENABLE_QT}")
message(STATUS "    ENABLE_SDL: ${ENABLE_SDL}")
message(STATUS "    ENABLE_GLES: ${ENABLE_GLES}")
message(STATUS "    ENABLE_THREADING: ${ENABLE_THREADING}")
message(STATUS "    ENABLE_PULSEAUDIO: ${ENABLE_PULSEAUDIO}")
message(STATUS "    ENABLE_JACK: ${ENABLE_JACK}")
message(STATUS "    ENABLE_LLVM: ${ENABLE_LLVM}")
message(STATUS "    ENABLE_EMSCRIPTEN: ${ENABLE_EMSCRIPTEN}")
message(STATUS "    USE_SYSTEM_GLM: ${USE_SYSTEM_GLM}")

add_subdirectory(src)
