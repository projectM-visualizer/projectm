PROJECT(qprojectM-pulseaudio)

set(QT_USE_OPENGL TRUE)

include(${QT_USE_FILE} FindPkgConfig.cmake)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(Qt4 REQUIRED)

# could be wrong
pkg_search_module(PROJECTM REQUIRED projectM qprojectM)

#pkg_search_module(PULSE_AUDIO REQUIRED pulseaudio)

# the variable "qprojectM_SRCS" contains all .cpp files of this project
set(qprojectM_pulseaudio_SRCS
  qprojectM-pulseaudio.cpp ConfigFile.h ConfigFile.cpp QPulseAudioThread.cpp
)

set(qprojectM_pulseaudio_MOC_HDRS
	QPulseAudioThread.hpp
)


# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
qt4_wrap_cpp(qprojectM_pulseaudio_MOC_SRCS ${qprojectM_pulseaudio_MOC_HDRS})

ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='"${PROJECTM_PREFIX}"')
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_NO_DEBUG)

INCLUDE_DIRECTORIES(${PROJECTM_INCLUDEDIR}  ${QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR})

LINK_DIRECTORIES(${PROJECTM_LDFLAGS} ${PULSEAUDIO_LDFLAGS})

ADD_EXECUTABLE(qprojectM-pulseaudio ${qprojectM_pulseaudio_SRCS} ${qprojectM_pulseaudio_MOC_SRCS})

TARGET_LINK_LIBRARIES(qprojectM-pulseaudio qprojectM projectM pulse-simple ${QT_QTGUI_LIBRARIES} ${QT_QTOPENGL_LIBRARIES} ${QT_QTCORE_LIBRARIES} ${QT_LIBRARIES})

INSTALL(TARGETS qprojectM-pulseaudio DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )