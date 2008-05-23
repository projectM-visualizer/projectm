project (PROJECTM_ROOT)

OPTION(INCLUDE-PROJECTM-QT "Build the projectM Qt4 user interface extension library (recommended)" ON)
OPTION(INCLUDE-PROJECTM-PULSEAUDIO "Build the projectM pulse audio client (recommended for fedora users)" ON)
OPTION(INCLUDE-PROJECTM-LIBVISUAL "Build the projectM libvisual client" ON)
OPTION(INCLUDE-PROJECTM-LIBVISUAL-ALSA "Build experimental libvisual / alsa input standalone client (NEW!)" OFF)
OPTION(INCLUDE-PROJECTM-JACK "Build the projectM JACK client" OFF)
OPTION(INCLUDE-PROJECTM-TEST "Build the projectM test suite" ON)
OPTION(INCLUDE-PROJECTM-XMMS "Build the projectM xmms module (deprecated, use audacious instead)" OFF)

add_subdirectory (projectM-engine)

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
