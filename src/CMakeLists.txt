project (projectM-complete)

cmake_minimum_required(VERSION 2.6.0)


set(PROJECTM_ROOT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if(EMSCRIPTEN)
  SET(EMSCRIPTEN-DEFAULT ON)
elseif(EMSCRIPTEN)
  SET(EMSCRIPTEN-DEFAULT OFF)
endif(EMSCRIPTEN)

OPTION(INCLUDE-PROJECTM-QT "Build the projectM Qt4 user interface extension library (recommended)" OFF)
OPTION(INCLUDE-PROJECTM-PULSEAUDIO "Build the projectM pulse audio client (recommended for fedora users)" OFF)
OPTION(INCLUDE-PROJECTM-LIBVISUAL "Build the projectM libvisual client" OFF)
OPTION(INCLUDE-PROJECTM-LIBVISUAL-ALSA "Build experimental libvisual / alsa input standalone client" OFF)
OPTION(INCLUDE-PROJECTM-JACK "Build the projectM JACK client" OFF)
OPTION(INCLUDE-PROJECTM-TEST "Build the projectM test suite" OFF)
OPTION(INCLUDE-PROJECTM-EMSCRIPTEN "Build projectM for the web (emscripten)" ${EMSCRIPTEN-DEFAULT})
OPTION(INCLUDE-PROJECTM-XMMS "Build the projectM xmms module (deprecated, use audacious instead)" OFF)
OPTION(INCLUDE-PROJECTM-SDL "Build the projectM SDL application" ON)
OPTION(INCLUDE-NATIVE-SAMPLES "Build the projectM native preset sample collection " ON)

add_subdirectory (libprojectM)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

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

if (INCLUDE-NATIVE-SAMPLES)
	add_subdirectory(NativePresets)
endif (INCLUDE-NATIVE-SAMPLES)
