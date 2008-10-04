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

ADD_LIBRARY(RovastarDriftingChaos SHARED RovastarDriftingChaos.cpp)
#ADD_LIBRARY(RovastarDarkSecret SHARED RovastarDarkSecret.cpp)
#ADD_LIBRARY(RovastarFractalSpiral SHARED RovastarFractalSpiral.cpp)
#ADD_LIBRARY(RovastarFractopiaFrantic SHARED RovastarFractopiaFranctic.cpp)

if (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")
   set(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM)
   set(PROJECTM_LINK ${PROJECTM_ROOT_BINARY_DIR}/libprojectM)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
   set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libprojectM)
   set(PROJECTM_LINK ${PROJECTM_LDFLAGS})
endif (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")

LINK_DIRECTORIES(${PROJECTM_LINK})

include_directories(${PROJECTM_INCLUDE} ${CMAKE_CURRENT_LIBRARY_DIR})

target_link_libraries(RovastarDriftingChaos projectM)

