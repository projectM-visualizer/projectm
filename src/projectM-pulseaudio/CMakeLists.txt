PROJECT(projectM-pulseaudio)

cmake_minimum_required(VERSION 2.4.0)

Include(cmake/CPack-projectM.cmake)

# Include the current dir as a source for find_package macro
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

set(QT_USE_OPENGL TRUE)

cmake_policy(SET CMP0005 OLD)

include(${QT_USE_FILE} FindPkgConfig.cmake)

# only search for these modules when not building monolithically

if (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
	pkg_search_module(LIBPROJECTM-QT REQUIRED libprojectM-qt)
	ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='\"${LIBPROJECTM_PREFIX}\"')
else(${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	set(LIBPROJECTM_FOUND true)
	set(LIBPROJECTM-QT_FOUND true)
	ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='\"${CMAKE_INSTALL_PREFIX}\"')
endif(${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(Qt4 REQUIRED)


pkg_search_module(LIBPULSE REQUIRED libpulse>=0.9.8)

if (LIBPROJECTM_FOUND)
MESSAGE (STATUS "[projectM-pulseaudio] projectM detected.")
else(LIBPROJECTM_FOUND)
MESSAGE (FATAL_ERROR "projectM NOT detected. Please install the projectM module or build from the top level projectM source directory.")
endif(LIBPROJECTM_FOUND)

if (LIBPROJECTM-QT_FOUND)
MESSAGE (STATUS "[projectM-pulseaudio] projectM-qt detected.")
else(LIBPROJECTM-QT_FOUND)
MESSAGE (FATAL_ERROR "projectM-qt NOT detected. Please install the projectM-qt module or build from the top level projectM source directory.")
endif(LIBPROJECTM-QT_FOUND)

if (LIBPULSE_FOUND)
MESSAGE(STATUS "[projectM-pulseaudio] pulse audio detected.")
else (LIBPULSE_FOUND)
MESSAGE(FATAL_ERROR "ERROR: Pulse Audio is NOT found. Please install pulse audio 0.9.8 or greater from www.pulseaudio.org.")
endif(LIBPULSE_FOUND)

# ftgl - OpenGL library to use arbitrary fonts
pkg_search_module(FTGL REQUIRED ftgl)
if (FTGL_FOUND)
  message(STATUS "[projectm-pulseaudio] ftgl library detected.")
  message(STATUS "FTGL_LIBRARIES = ${FTGL_LIBRARIES}")
  message(STATUS "FTGL_LIBRARY_DIRS = ${FTGL_LIBRARY_DIRS}")
  include_directories(${FTGL_INCLUDE_DIRS})
  link_directories(${FTGL_LIBRARY_DIRS})
else (FTGL_FOUND USE)
##  message(FATAL_ERROR "ERROR: ftgl library not detected. Please install it from ftgl.wiki.sourceforge.net")
endif (FTGL_FOUND)

set(projectM_pulseaudio_SRCS
  qprojectM-pulseaudio.cpp ConfigFile.h ConfigFile.cpp QPulseAudioThread.cpp QPulseAudioDeviceChooser.cpp QPulseAudioDeviceModel.cpp
)

set(projectM_pulseaudio_MOC_HDRS
	QPulseAudioThread.hpp QPulseAudioDeviceChooser.hpp QPulseAudioDeviceModel.hpp
)

set(projectM_pulseaudio_UIS
    PulseDeviceChooserDialog.ui
)


qt4_wrap_ui(projectM_pulseaudio_UIS_H ${projectM_pulseaudio_UIS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
qt4_wrap_cpp(projectM_pulseaudio_MOC_SRCS ${projectM_pulseaudio_MOC_HDRS})

ADD_DEFINITIONS(-DLINUX)
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_NO_DEBUG)


if (${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")
	set(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM ${PROJECTM_ROOT_SOURCE_DIR}/projectM-qt)
	set(PROJECTM_LINK ${PROJECTM_ROOT_BINARY_DIR}/libprojectM ${PROJECTM_ROOT_BINARY_DIR}/projectM-qt)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libprojectM ${LIBPROJECTM-QT_INCLUDEDIR}/libprojectM-qt)
	set(PROJECTM_LINK ${LIBPROJECTM_LDFLAGS} ${LIBPROJECTM-QT_LDFLAGS})
	# projectM-qt places itself in same location as (lib)projectM
	# whether it should is a whole other issue
endif (${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")

INCLUDE_DIRECTORIES(${PROJECTM_INCLUDE}
		${QT_INCLUDE_DIR}
		${QT_QTGUI_INCLUDE_DIR}
		${QT_QTOPENGL_INCLUDE_DIR}
		${QT_QTCORE_INCLUDE_DIR}
		${LIBPULSE_INCLUDEDIR}
		${CMAKE_CURRENT_BINARY_DIR}
		${CMAKE_BINARY_DIR}/projectM-pulseaudio
)

LINK_DIRECTORIES(${PROJECTM_LINK})

if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

ADD_EXECUTABLE(projectM-pulseaudio ${projectM_pulseaudio_SRCS} ${projectM_pulseaudio_MOC_SRCS} ${projectM_pulseaudio_UIS_H})

TARGET_LINK_LIBRARIES(projectM-pulseaudio projectM-qt projectM pulse ${QT_QTGUI_LIBRARIES} ${QT_QTOPENGL_LIBRARIES} ${QT_QTCORE_LIBRARIES} ${QT_LIBRARIES} ${GLEW_LIBRARY} ${FTGL_LIBRARIES})

INSTALL(TARGETS projectM-pulseaudio DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
install(FILES "projectM-pulseaudio.desktop" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
