PROJECT(projectM-tests)

ADD_EXECUTABLE(projectM-test projectM-test.cpp sdltoprojectM.h video_init.cpp ConfigFile.h ConfigFile.cpp getConfigFilename.cpp getConfigFilename.h)
ADD_EXECUTABLE(projectM-test-memleak projectM-test-memleak.cpp sdltoprojectM.h video_init.cpp ConfigFile.h ConfigFile.cpp)

ADD_EXECUTABLE(projectM-test-texture projectM-test-texture.cpp sdltoprojectM.h video_init.cpp ConfigFile.h ConfigFile.cpp)

INCLUDE(FindPkgConfig.cmake)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(SDL)

if (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
else (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	set(LIBPROJECTM_FOUND true)
endif(${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})

ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='"${PROJECTM_PREFIX}"')


if (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")
   set(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/projectM-engine)
   set(PROJECTM_LINK ${PROJECTM_ROOT_BINARY_DIR}/projectM-engine)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
   set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libprojectM)
   set(PROJECTM_LINK ${LIBPROJECTM_LDFLAGS})
endif (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")

INCLUDE_DIRECTORIES(${PROJECTM_INCLUDE} ${SDL_INCLUDE_DIR} ${GTK_INCLUDE_DIR})
LINK_DIRECTORIES(${PROJECTM_LINK})

TARGET_LINK_LIBRARIES(projectM-test projectM  ${SDL_LIBRARY} ${GTK_LIBRARIES})
TARGET_LINK_LIBRARIES(projectM-test-memleak projectM  ${SDL_LIBRARY} ${GTK_LIBRARIES})
TARGET_LINK_LIBRARIES(projectM-test-texture projectM  ${SDL_LIBRARY} ${GTK_LIBRARIES})

INSTALL(TARGETS projectM-test projectM-test-texture DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
