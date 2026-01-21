# Install script for directory: /home/runner/work/Project-M/Project-M/src/api

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/projectM-4" TYPE FILE FILES
    "/home/runner/work/Project-M/Project-M/build-test-with-openmp/src/api/include/projectM-4/projectM_export.h"
    "/home/runner/work/Project-M/Project-M/build-test-with-openmp/src/api/include/projectM-4/version.h"
    "/home/runner/work/Project-M/Project-M/src/api/include/projectM-4/audio.h"
    "/home/runner/work/Project-M/Project-M/src/api/include/projectM-4/callbacks.h"
    "/home/runner/work/Project-M/Project-M/src/api/include/projectM-4/core.h"
    "/home/runner/work/Project-M/Project-M/src/api/include/projectM-4/debug.h"
    "/home/runner/work/Project-M/Project-M/src/api/include/projectM-4/logging.h"
    "/home/runner/work/Project-M/Project-M/src/api/include/projectM-4/memory.h"
    "/home/runner/work/Project-M/Project-M/src/api/include/projectM-4/parameters.h"
    "/home/runner/work/Project-M/Project-M/src/api/include/projectM-4/projectM.h"
    "/home/runner/work/Project-M/Project-M/src/api/include/projectM-4/render_opengl.h"
    "/home/runner/work/Project-M/Project-M/src/api/include/projectM-4/touch.h"
    "/home/runner/work/Project-M/Project-M/src/api/include/projectM-4/types.h"
    "/home/runner/work/Project-M/Project-M/src/api/include/projectM-4/user_sprites.h"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/runner/work/Project-M/Project-M/build-test-with-openmp/src/api/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
