PROJECT(qprojectM-pulseaudio)

set(QT_USE_OPENGL TRUE)

include(${QT_USE_FILE} FindPkgConfig.cmake)

# only search for these modules when not building monolithically

if (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
	pkg_search_module(LIBQPROJECTM REQUIRED libqprojectM)
else(${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	set(LIBPROJECTM_FOUND true)
	set(LIBQPROJECTM_FOUND true)
endif(${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(Qt4 REQUIRED)


pkg_search_module(LIBPULSE REQUIRED libpulse)

if (LIBPROJECTM_FOUND)
MESSAGE (STATUS "projectM detected.")
else(LIBPROJECTM_FOUND)
MESSAGE (FATAL_ERROR "projectM NOT detected. Please install the projectM module or build from the top level projectM source directory.")
endif(LIBPROJECTM_FOUND)

if (LIBQPROJECTM_FOUND)
MESSAGE (STATUS "qprojectM detected.")
else(LIBQPROJECTM_FOUND)
MESSAGE (FATAL_ERROR "qprojectM NOT detected. Please install the qprojectM module or build from the top level projectM source directory.")
endif(LIBQPROJECTM_FOUND)

if (LIBPULSE_FOUND)
MESSAGE(STATUS "Pulse audio detected")
else (LIBPULSE_FOUND)
MESSAGE(FATAL_ERROR "ERROR: Pulse Audio is NOT found. Please install pulse audio 0.9.8 or greater from www.pulseaudio.org.")
endif(LIBPULSE_FOUND)


# the variable "qprojectM_SRCS" contains all .cpp files of this project
set(qprojectM_pulseaudio_SRCS
  qprojectM-pulseaudio.cpp ConfigFile.h ConfigFile.cpp QPulseAudioThread.cpp QPulseAudioDeviceChooser.cpp QPulseAudioDeviceModel.cpp
)

set(qprojectM_pulseaudio_MOC_HDRS
	QPulseAudioThread.hpp QPulseAudioDeviceChooser.hpp QPulseAudioDeviceModel.hpp
)

set(qprojectM_pulseaudio_UIS
    PulseDeviceChooserDialog.ui
)


qt4_wrap_ui(qprojectM_pulseaudio_UIS_H ${qprojectM_pulseaudio_UIS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
qt4_wrap_cpp(qprojectM_pulseaudio_MOC_SRCS ${qprojectM_pulseaudio_MOC_HDRS})

ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='"${PROJECTM_PREFIX}"')
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_NO_DEBUG)


if (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")
   set(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/projectM-engine ${PROJECTM_ROOT_SOURCE_DIR}/qprojectM)
   set(PROJECTM_LINK ${PROJECTM_ROOT_BINARY_DIR}/projectM-engine ${PROJECTM_ROOT_BINARY_DIR}/qprojectM)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
   set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libprojectM ${LIBQPROJECTM_INCLUDEDIR}/libqprojectM)
   set(PROJECTM_LINK ${LIBPROJECTM_LDFLAGS} ${LIBQPROJECTM_LDFLAGS})
# qprojectM places itself in same location as (lib)projectM 
# whether it should is a whole other issue
endif (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")

INCLUDE_DIRECTORIES(${PROJECTM_INCLUDE} ${QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${LIBPULSE_INCLUDEDIR})

LINK_DIRECTORIES(${PROJECTM_LINK} ${PULSEAUDIO_LDFLAGS})

ADD_EXECUTABLE(qprojectM-pulseaudio ${qprojectM_pulseaudio_SRCS} ${qprojectM_pulseaudio_MOC_SRCS} ${qprojectM_pulseaudio_UIS_H})

TARGET_LINK_LIBRARIES(qprojectM-pulseaudio qprojectM projectM pulse ${QT_QTGUI_LIBRARIES} ${QT_QTOPENGL_LIBRARIES} ${QT_QTCORE_LIBRARIES} ${QT_LIBRARIES})

INSTALL(TARGETS qprojectM-pulseaudio DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )