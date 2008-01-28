PROJECT(qprojectM-pulseaudio)

set(QT_USE_OPENGL TRUE)

include(${QT_USE_FILE} FindPkgConfig.cmake)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(Qt4 REQUIRED)

pkg_search_module(PROJECTM REQUIRED libprojectM)
pkg_search_module(PROJECTM REQUIRED libqprojectM)

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

INCLUDE_DIRECTORIES(${PROJECTM_ROOT_SOURCE_DIR}/projectM-engine ${PROJECTM_ROOT_SOURCE_DIR}/qprojectM ${QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR})

LINK_DIRECTORIES(${PROJECTM_ROOT_BINARY_DIR}/projectM-engine ${PROJECTM_ROOT_BINARY_DIR}/qprojectM ${PROJECTM_LDFLAGS} ${PULSEAUDIO_LDFLAGS})

ADD_EXECUTABLE(qprojectM-pulseaudio ${qprojectM_pulseaudio_SRCS} ${qprojectM_pulseaudio_MOC_SRCS} ${qprojectM_pulseaudio_UIS_H})

TARGET_LINK_LIBRARIES(qprojectM-pulseaudio qprojectM projectM pulse pulse-browse ${QT_QTGUI_LIBRARIES} ${QT_QTOPENGL_LIBRARIES} ${QT_QTCORE_LIBRARIES} ${QT_LIBRARIES})

INSTALL(TARGETS qprojectM-pulseaudio DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )