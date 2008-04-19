PROJECT(projectM_libvisual)
ADD_LIBRARY(projectM_libvisual SHARED actor_projectM.cpp lvtoprojectM.h ConfigFile.h ConfigFile.cpp)

INCLUDE(FindPkgConfig.cmake)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(SDL)

if (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
else (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	set(LIBPROJECTM_FOUND true)
endif(${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})

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

ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='"${PROJECTM_PREFIX}"')

if (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")
   set(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/projectM-engine)
   set(PROJECTM_LINK ${PROJECTM_ROOT_BINARY_DIR}/projectM-engine)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
   set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libprojectM)
   set(PROJECTM_LINK ${LIBPROJECTM_LDFLAGS})
endif (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")

INCLUDE_DIRECTORIES(${PROJECTM_INCLUDE} ${LIBVISUAL_INCLUDEDIR} ${SDL_INCLUDE_DIR})
LINK_DIRECTORIES(${PROJECTM_LINK} ${LIBVISUAL_LDFLAGS})

TARGET_LINK_LIBRARIES(projectM_libvisual projectM  ${SDL_LIBRARY} ${LIBVISUAL_LIBRARIES})

INSTALL(TARGETS projectM_libvisual DESTINATION ${LIBVISUAL_PLUGINSDIR}/actor )
