
project(projectM-qt) # the name of your project
cmake_minimum_required(VERSION 2.4.0)
cmake_policy(SET CMP0005 OLD)

include(cmake/CPack-projectM.cmake)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

include(FindPkgConfig.cmake)
if (${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")
	set(LIBPROJECTM_FOUND true)
else(${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")
	pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
endif(${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")

OPTION(BUILD_PROJECTM_QT_STATIC "Build a static library of projectM-qt rather than the usual shared library format" OFF)

# projectM-Qt version block
set(PROJECTM_QT_VERSION 1.10)
set(PROJECTM_QT_ABI_LEVEL 1)
#

find_package(Qt4 REQUIRED) # find and setup Qt4 for this project
include(${QT_USE_FILE})

if (QT4_FOUND)
MESSAGE(STATUS "[projectM-qt] Qt4 detected.")
else (QT4_FOUND)
MESSAGE(FATAL_ERROR "Qt4 not detected. Please install Qt4 before continuing (visit www.trolltech.org for more info)")
endif(QT4_FOUND)
set(QT_USE_OPENGL TRUE)

if (LIBPROJECTM_FOUND)
	MESSAGE(STATUS "[projectM-qt] projectM detected.")
else (LIBPROJECTM_FOUND)
	MESSAGE(FATAL_ERROR "projectM not detected! Please install the projectM module or build from top level projectM source directory.")
endif(LIBPROJECTM_FOUND)


CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/libprojectM-qt.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/libprojectM-qt.pc" @ONLY)

ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='\"${PROJECTM_PREFIX}\"')
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
#ADD_DEFINITIONS(-DQT_STATIC)

# the variable "projectM-qt_SRCS" contains all .cpp files of this project
set(projectM-qt_SRCS
    qprojectm_mainwindow.cpp
    qprojectm_mainwindow.hpp
    configfile.hpp configfile.cpp
    qpresetfiledialog.hpp
    qplaylistfiledialog.cpp
    qplaylistfiledialog.hpp
    qplaylistmodel.cpp
    qplaylistmodel.hpp
    qxmlplaylisthandler.hpp
    qprojectmconfigdialog.cpp
    qprojectmconfigdialog.hpp
    qplaylisttableview.hpp
    qpresettextedit.cpp
    qpresettextedit.hpp
    qpreseteditordialog.cpp
    qpreseteditordialog.hpp
    qprojectm.hpp
    qprojectmwidget.hpp
)


set(projectM-qt_MOC_HDRS
    qprojectm_mainwindow.hpp
    qpresetfiledialog.hpp
    qplaylistfiledialog.hpp
    qplaylistmodel.hpp
    qprojectmconfigdialog.hpp
    qplaylisttableview.hpp
    qpresettextedit.hpp
    qpreseteditordialog.hpp
    qprojectm.hpp
    qprojectmwidget.hpp
)

set(projectM-qt_UIS
    qprojectm_mainwindow.ui
    qprojectmconfigdialog.ui
    qpreseteditordialog.ui
)

# Qt resource file
set( projectM-qt_RESOURCES
    application.qrc
)

# compile resources
qt4_add_resources( projectM-qt_RC_SRCS ${projectM-qt_RESOURCES} )

qt4_wrap_ui(projectM-qt_UIS_H ${projectM-qt_UIS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
qt4_wrap_cpp(projectM-qt_MOC_SRCS ${projectM-qt_MOC_HDRS})

if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (BUILD_PROJECTM_QT_STATIC)
ADD_LIBRARY(projectM-qt STATIC ${projectM-qt_SRCS} ${projectM-qt_RC_SRCS} ${projectM-qt_MOC_SRCS} ${projectM-qt_UIS_H})
else(BUILD_PROJECTM_QT_STATIC)
ADD_LIBRARY(projectM-qt SHARED ${projectM-qt_SRCS} ${projectM-qt_RC_SRCS} ${projectM-qt_MOC_SRCS} ${projectM-qt_UIS_H})
endif(BUILD_PROJECTM_QT_STATIC)

set_property(TARGET projectM-qt PROPERTY LINK_INTERFACE_LIBRARIES "")
set_property(TARGET projectM-qt PROPERTY IMPORTED_LINK_DEPENDENT_LIBRARIES "")
SET_TARGET_PROPERTIES(projectM-qt PROPERTIES VERSION ${PROJECTM_QT_VERSION} SOVERSION ${PROJECTM_QT_ABI_LEVEL})

if (${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")
   set(PROJECTM_INCLUDE ${projectM-complete_SOURCE_DIR}/libprojectM)
   set(PROJECTM_LINK ${projectM-complete_BINARY_DIR}/libprojectM)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
   set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libprojectM)
   set(PROJECTM_LINK ${PROJECTM_LDFLAGS})
endif (${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")

LINK_DIRECTORIES(${PROJECTM_LINK})

include_directories(${PROJECTM_INCLUDE} ${QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${CMAKE_CURRENT_LIBRARY_DIR})

# link the "qprojectM" target against the Qt libraries. which libraries exactly, is defined by the "include(${QT_USE_FILE})" line above, which sets up this variable.
target_link_libraries(projectM-qt projectM GLEW GL ${QT_QTGUI_LIBRARIES} ${QT_QTOPENGL_LIBRARIES} ${QT_QTXML_LIBRARIES} ${QT_LIBRARIES})

install(FILES qprojectm_mainwindow.hpp DESTINATION include/libprojectM-qt)
install(TARGETS projectM-qt DESTINATION lib)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libprojectM-qt.pc" DESTINATION lib/pkgconfig)
install(FILES "images/icons/prjm16-transparent.svg" DESTINATION	${CMAKE_INSTALL_PREFIX}/share/pixmaps)
