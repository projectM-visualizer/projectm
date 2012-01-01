PROJECT(projectM-tests)
cmake_minimum_required(VERSION 2.4.0)
cmake_policy(SET CMP0005 OLD)

if(COMMAND cmake_policy)
       cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


INCLUDE(cmake/CPack-projectM.cmake)

ADD_EXECUTABLE(projectM-test         projectM-test.cpp         sdltoprojectM.h video_init.cpp ConfigFile.h ConfigFile.cpp getConfigFilename.cpp getConfigFilename.h)
ADD_EXECUTABLE(projectM-test-memleak projectM-test-memleak.cpp sdltoprojectM.h video_init.cpp ConfigFile.h ConfigFile.cpp getConfigFilename.cpp getConfigFilename.h)
ADD_EXECUTABLE(projectM-test-texture projectM-test-texture.cpp sdltoprojectM.h video_init.cpp ConfigFile.h ConfigFile.cpp getConfigFilename.h getConfigFilename.cpp)

INCLUDE(FindPkgConfig.cmake)

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

if (${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")
   set(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
   set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libprojectM)
   set(PROJECTM_LINK ${LIBPROJECTM_LDFLAGS})
endif (${CMAKE_PROJECT_NAME} MATCHES "projectM-complete")

INCLUDE_DIRECTORIES(${PROJECTM_INCLUDE} ${SDL_INCLUDE_DIR} ${GTK_INCLUDE_DIR})
LINK_DIRECTORIES(${PROJECTM_LINK})

TARGET_LINK_LIBRARIES(projectM-test projectM  ${SDL_LIBRARY})
TARGET_LINK_LIBRARIES(projectM-test-memleak projectM  ${SDL_LIBRARY} )
TARGET_LINK_LIBRARIES(projectM-test-texture projectM  ${SDL_LIBRARY} )

INSTALL(TARGETS projectM-test projectM-test-texture DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
