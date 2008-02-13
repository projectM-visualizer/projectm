# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
project(qprojectM) # the name of your project

cmake_minimum_required(VERSION 2.4.0)

include(FindPkgConfig.cmake)

if (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")
	set(LIBPROJECTM_FOUND true)
else(${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")
	pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
endif(${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")


find_package(Qt4 REQUIRED) # find and setup Qt4 for this project
include(${QT_USE_FILE})

if (QT4_FOUND)
MESSAGE(STATUS "Qt4 Detected fine.")
else (QT4_FOUND)
MESSAGE(FATAL_ERROR "Qt4 not detected. Please install Qt4 before continuing (visit www.trolltech.org for more info)")
endif(QT4_FOUND)
set(QT_USE_OPENGL TRUE)

if (LIBPROJECTM_FOUND)
	MESSAGE(STATUS "projectM detected.")
else (LIBPROJECTM_FOUND)
	MESSAGE(FATAL_ERROR "projectM not detected! Please install the projectM module or build from top level projectM source directory.")
endif(LIBPROJECTM_FOUND)


CONFIGURE_FILE("libqprojectM.pc.in" "libqprojectM.pc" @ONLY)

ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='"${PROJECTM_PREFIX}"')
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

# the variable "qprojectM_SRCS" contains all .cpp files of this project
set(qprojectM_SRCS
    QProjectM_MainWindow.cpp
    QProjectM_MainWindow.hpp
    ConfigFile.h ConfigFile.cpp
    QPresetFileDialog.hpp
    QPlaylistFileDialog.hpp
    QPlaylistModel.cpp
    QPlaylistModel.hpp
    QXmlPlaylistHandler.hpp
    QProjectMConfigDialog.cpp
    QProjectMConfigDialog.hpp
)


set(qprojectM_MOC_HDRS
    QProjectM_MainWindow.hpp
    QPresetFileDialog.hpp
    QPlaylistFileDialog.hpp
    QPlaylistModel.hpp
    QProjectMConfigDialog.hpp
)

set(qprojectM_UIS
    QProjectM_MainWindow.ui
    QProjectMConfigDialog.ui
)

# Qt resource file
set( qprojectM_RESOURCES
    application.qrc
)

# compile resources
qt4_add_resources( qprojectM_RC_SRCS ${qprojectM_RESOURCES} )

qt4_wrap_ui(qprojectM_UIS_H ${qprojectM_UIS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
qt4_wrap_cpp(qprojectM_MOC_SRCS ${qprojectM_MOC_HDRS})

ADD_LIBRARY(qprojectM SHARED ${qprojectM_SRCS} ${qprojectM_RC_SRCS} ${qprojectM_MOC_SRCS} ${qprojectM_UIS_H})

SET_TARGET_PROPERTIES(qprojectM PROPERTIES VERSION 1.10 SOVERSION 1)

if (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")
   set(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/projectM-engine)
   set(PROJECTM_LINK ${PROJECTM_ROOT_BINARY_DIR}/projectM-engine)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
   set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libprojectM)
   set(PROJECTM_LINK ${PROJECTM_LDFLAGS})
endif (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")

LINK_DIRECTORIES(${PROJECTM_LINK})

include_directories(${PROJECTM_INCLUDE} ${QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${CMAKE_CURRENT_LIBRARY_DIR})

# link the "qprojectM" target against the Qt libraries. which libraries exactly, is defined by the "include(${QT_USE_FILE})" line above, which sets up this variable.
target_link_libraries(qprojectM projectM ${QT_QTGUI_LIBRARIES} ${QT_QTOPENGL_LIBRARIES} ${QT_QTXML_LIBRARIES} ${QT_LIBRARIES})

install(FILES QProjectM_MainWindow.hpp DESTINATION include/libqprojectM)
install(TARGETS qprojectM DESTINATION lib)
install(FILES "${CMAKE_BINARY_DIR}/libqprojectM.pc" DESTINATION ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/pkgconfig)