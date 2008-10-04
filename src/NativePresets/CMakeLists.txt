
project(NativePresets)
cmake_minimum_required(VERSION 2.4.0)

include(FindPkgConfig.cmake)

if (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")
	set(LIBPROJECTM_FOUND true)
else(${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")
	pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
endif(${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")

if (LIBPROJECTM_FOUND)
	MESSAGE(STATUS "[NativePresets] projectM detected.")
else (LIBPROJECTM_FOUND)
	MESSAGE(FATAL_ERROR "projectM not detected! Please install the projectM module or build from top level projectM source directory.")
endif(LIBPROJECTM_FOUND)


ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='"${PROJECTM_PREFIX}"')

# the variable "projectM-qt_SRCS" contains all .cpp files of this project
set(NativePresets_HEADERS

)
ADD_LIBRARY(projectM-qt SHARED ${projectM-qt_SRCS} ${projectM-qt_RC_SRCS} ${projectM-qt_MOC_SRCS} 
SET_TARGET_PROPERTIES(projectM-qt PROPERTIES VERSION 1.10 SOVERSION 1)

if (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")
   set(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM)
   set(PROJECTM_LINK ${PROJECTM_ROOT_BINARY_DIR}/libprojectM)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
   set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libprojectM)
   set(PROJECTM_LINK ${PROJECTM_LDFLAGS})
endif (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")

LINK_DIRECTORIES(${PROJECTM_LINK})

include_directories(${PROJECTM_INCLUDE} ${CMAKE_CURRENT_LIBRARY_DIR})

# link the "qprojectM" target against the Qt libraries. which libraries exactly, is defined by the "include(${QT_USE_FILE})" line above, which sets up this variable.
target_link_libraries(projectM-qt projectM ${QT_QTGUI_LIBRARIES} ${QT_QTOPENGL_LIBRARIES} ${QT_QTXML_LIBRARIES} ${QT_LIBRARIES})

install(FILES qprojectm_mainwindow.hpp DESTINATION include/libprojectM-qt)
install(TARGETS projectM-qt DESTINATION lib${LIB_SUFFIX})
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/libprojectM-qt.pc" DESTINATION ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/pkgconfig)
install(FILES "images/icons/prjm16-transparent.svg" DESTINATION	${CMAKE_INSTALL_PREFIX}/share/pixmaps)
