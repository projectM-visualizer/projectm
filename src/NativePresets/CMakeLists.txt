project(NativePresets)
cmake_minimum_required(VERSION 2.4.0)
cmake_policy(SET CMP0005 OLD)

include(FindPkgConfig.cmake)

Include(cmake/CPack-projectM.cmake)

if (${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")
	set(LIBPROJECTM_FOUND true)
else(${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")
	pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
endif(${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")

if (LIBPROJECTM_FOUND)
	MESSAGE(STATUS "[NativePresets] projectM detected.")
else (LIBPROJECTM_FOUND)
	MESSAGE(FATAL_ERROR "projectM not detected! Please install the projectM module or build from top level projectM source directory.")
endif(LIBPROJECTM_FOUND)


ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='\"${PROJECTM_PREFIX}\"')


if(COMMAND cmake_policy)
       cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
ADD_LIBRARY(RovastarDriftingChaos SHARED RovastarDriftingChaos.cpp)
ADD_LIBRARY(RovastarDarkSecret SHARED RovastarDarkSecret.cpp)
ADD_LIBRARY(RovastarFractalSpiral SHARED RovastarFractalSpiral.cpp)
ADD_LIBRARY(RovastarFractopiaFrantic SHARED RovastarFractopiaFrantic.cpp)
ADD_LIBRARY(MstressJuppyDancer SHARED MstressJuppyDancer.cpp)

# Doesn't build yet
#ADD_LIBRARY(RLGFractalDrop7c SHARED RLGFractalDrop7c.cpp)

if (${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")
   set(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM)
   set(PROJECTM_LINK ${PROJECTM_ROOT_BINARY_DIR}/libprojectM)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
   set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libprojectM)
   set(PROJECTM_LINK ${PROJECTM_LDFLAGS})
endif (${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")

LINK_DIRECTORIES(${PROJECTM_LINK} ${Renderer_BINARY_DIR} ${NativePresetFactory_BINARY_DIR})

include_directories(${PROJECTM_INCLUDE} ${Renderer_SOURCE_DIR} ${NativePresetFactory_SOURCE_DIR} ${CMAKE_CURRENT_LIBRARY_DIR})

target_link_libraries(RovastarDarkSecret Renderer NativePresetFactory projectM)
target_link_libraries(RovastarFractopiaFrantic Renderer NativePresetFactory projectM)
target_link_libraries(RovastarFractalSpiral Renderer NativePresetFactory projectM)
target_link_libraries(RovastarDriftingChaos Renderer NativePresetFactory projectM)
target_link_libraries(MstressJuppyDancer Renderer NativePresetFactory projectM)
# Doesn't build yet
#target_link_libraries(RLGFractalDrop7c Renderer NativePresetFactory projectM)
