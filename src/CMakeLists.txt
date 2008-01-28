project (PROJECTM_ROOT)

OPTION(INCLUDE-QPROJECTM "Build the projectM qt4 user interface extension library (recommended)" ON)
OPTION(INCLUDE-QPROJECTM-PULSEAUDIO "Build the projectM qt4 / pulse audio client (recommended for fedora users)" ON)
OPTION(INCLUDE-PROJECTM-LIBVISUAL "Build the projectM libvisual client" ON)
OPTION(INCLUDE-QPROJECTM-JACK "Build the projectM qt4 / JACK client" OFF)
OPTION(INCLUDE-PROJECTM-JACK "Build the projectM sdl / JACK client" OFF)
OPTION(INCLUDE-PROJECTM-TEST "Build the projectM test suite" ON)
OPTION(INCLUDE-PROJECTM-XMMS "Build the projectM xmms module (deprecrated)" OFF)

add_subdirectory (projectM-engine)

if(INCLUDE-PROJECTM-TEST)
	add_subdirectory (projectM-test)
endif(INCLUDE-PROJECTM-TEST)

if (INCLUDE-QPROJECTM)
	add_subdirectory (qprojectM)
endif(INCLUDE-QPROJECTM)

if (INCLUDE-QPROJECTM-JACK)
	add_subdirectory (qprojectM-jack)
endif (INCLUDE-QPROJECTM-JACK)

if (INCLUDE-PROJECTM-JACK)
	add_subdirectory (projectM-jack)
endif (INCLUDE-PROJECTM-JACK)

if (INCLUDE-QPROJECTM-PULSEAUDIO)
	add_subdirectory (qprojectM-pulseaudio)
endif (INCLUDE-QPROJECTM-PULSEAUDIO)

if (INCLUDE-PROJECTM-XMMS)
	add_subdirectory (projectM-xmms)
endif (INCLUDE-PROJECTM-XMMS)

if (INCLUDE-PROJECTM-LIBVISUAL)
	add_subdirectory (projectM-libvisual)
endif (INCLUDE-PROJECTM-LIBVISUAL)
