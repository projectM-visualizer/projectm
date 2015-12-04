project (projectM-complete)

cmake_minimum_required(VERSION 2.4.0)

OPTION(INCLUDE-PROJECTM-SDL "Build the projectM SDL application" ON)
OPTION(INCLUDE-PROJECTM-QT "Build the projectM Qt4 user interface extension library (recommended)" OFF)
OPTION(INCLUDE-PROJECTM-PULSEAUDIO "Build the projectM pulse audio client (recommended for fedora users)" OFF)
OPTION(INCLUDE-PROJECTM-LIBVISUAL "Build the projectM libvisual client" OFF)
OPTION(INCLUDE-PROJECTM-LIBVISUAL-ALSA "Build experimental libvisual / alsa input standalone client (NEW!)" OFF)
OPTION(INCLUDE-PROJECTM-JACK "Build the projectM JACK client" OFF)
OPTION(INCLUDE-PROJECTM-TEST "Build the projectM test suite" OFF)
OPTION(INCLUDE-PROJECTM-EMSCRIPTEN "Build projectM for the web (emscripten)" OFF)
OPTION(INCLUDE-PROJECTM-XMMS "Build the projectM xmms module (deprecated, use audacious instead)" OFF)
OPTION(INCLUDE-NATIVE-PRESETS "Build the projectM native preset sample collection " ON)

set(PROJECTM_ROOT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if(EMSCRIPTEN)
  SET(INCLUDE-PROJECTM-QT OFF)
  SET(INCLUDE-PROJECTM-PULSEAUDIO OFF)
  SET(INCLUDE-PROJECTM-LIBVISUAL OFF)
  SET(INCLUDE-PROJECTM-TEST OFF)
  SET(INCLUDE-PROJECTM-EMSCRIPTEN ON)
  SET(INCLUDE-NATIVE-PRESETS OFF)
endif(EMSCRIPTEN)

# sensible OSX defaults
if(APPLE)
  SET(INCLUDE-PROJECTM-QT OFF)
  SET(INCLUDE-PROJECTM-PULSEAUDIO OFF)
  SET(INCLUDE-PROJECTM-LIBVISUAL OFF)
  SET(INCLUDE-PROJECTM-TEST OFF)
  SET(INCLUDE-PROJECTM-SDL ON)
  SET(INCLUDE-NATIVE-PRESETS OFF)
endif(APPLE)

add_subdirectory (libprojectM)

if(INCLUDE-PROJECTM-EMSCRIPTEN)
  add_subdirectory (projectM-emscripten)
endif(INCLUDE-PROJECTM-EMSCRIPTEN)

if(INCLUDE-PROJECTM-TEST)
  add_subdirectory (projectM-test)
endif(INCLUDE-PROJECTM-TEST)

if (INCLUDE-PROJECTM-QT)
	add_subdirectory (projectM-qt)
endif(INCLUDE-PROJECTM-QT)

if (INCLUDE-PROJECTM-JACK)
	add_subdirectory (projectM-jack)
endif (INCLUDE-PROJECTM-JACK)

if (INCLUDE-PROJECTM-PULSEAUDIO)
	add_subdirectory (projectM-pulseaudio)
endif (INCLUDE-PROJECTM-PULSEAUDIO)

if (INCLUDE-PROJECTM-XMMS)
	add_subdirectory (projectM-xmms)
endif (INCLUDE-PROJECTM-XMMS)

if (INCLUDE-PROJECTM-LIBVISUAL)
	add_subdirectory (projectM-libvisual)
endif (INCLUDE-PROJECTM-LIBVISUAL)

if (INCLUDE-PROJECTM-LIBVISUAL-ALSA)
  add_subdirectory (projectM-libvisual-alsa)
endif (INCLUDE-PROJECTM-LIBVISUAL-ALSA)

if (INCLUDE-PROJECTM-SDL)
  add_subdirectory (projectM-sdl)
endif (INCLUDE-PROJECTM-SDL)

if (INCLUDE-NATIVE-PRESETS)
	add_subdirectory(NativePresets)
endif (INCLUDE-NATIVE-PRESETS)
