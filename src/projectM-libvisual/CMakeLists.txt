PROJECT(projectM_libvisual)
cmake_minimum_required(VERSION 2.4.0)

INCLUDE(cmake/CPack-projectM.cmake)

ADD_LIBRARY(projectM_libvisual SHARED actor_projectM.cpp lvtoprojectM.h ConfigFile.h ConfigFile.cpp)

INCLUDE(FindPkgConfig.cmake)
cmake_policy(SET CMP0005 OLD)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(SDL)

if (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
	ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='\"${LIBPROJECTM_PREFIX}\"')
else (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	set(LIBPROJECTM_FOUND true)
	ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='\"${CMAKE_INSTALL_PREFIX}\"')
endif(${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})

ADD_DEFINITIONS(-DLINUX)

pkg_search_module(LIBVISUAL REQUIRED libvisual-0.4)

if (LIBPROJECTM_FOUND)
	MESSAGE (STATUS "[projectM-libvisual] projectM detected.")
else(LIBPROJECTM_FOUND)
	MESSAGE (FATAL_ERROR "projectM NOT detected. Please install the projectM module or build from the top level projectM source directory.")
endif(LIBPROJECTM_FOUND)

if(LIBVISUAL_FOUND)
	MESSAGE(STATUS "[projectM-libvisual] libvisual detected.")
else(LIBVISUAL_FOUND)
	MESSAGE(FATAL_ERROR "libvisual 0.4 not found! Please visit http://libvisual.sf.net and download the module.")
endif(LIBVISUAL_FOUND)


if (${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")
   set(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM)
   set(PROJECTM_LINK ${PROJECTM_ROOT_BINARY_DIR}/libprojectM)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
   set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libprojectM)
   set(PROJECTM_LINK ${LIBPROJECTM_LDFLAGS})
endif (${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")

INCLUDE_DIRECTORIES(${PROJECTM_INCLUDE} ${LIBVISUAL_INCLUDEDIR} ${SDL_INCLUDE_DIR})
LINK_DIRECTORIES(${PROJECTM_LINK})

TARGET_LINK_LIBRARIES(projectM_libvisual projectM  ${SDL_LIBRARY} ${LIBVISUAL_LIBRARIES})

INSTALL(TARGETS projectM_libvisual DESTINATION ${LIBVISUAL_PLUGINSDIR}/actor )
