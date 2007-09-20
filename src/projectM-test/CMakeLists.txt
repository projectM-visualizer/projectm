PROJECT(projectM-tests)

ADD_EXECUTABLE(projectM-test projectM-test.cpp sdltoprojectM.h video_init.cpp ConfigFile.h ConfigFile.cpp)
ADD_EXECUTABLE(projectM-test-texture projectM-test-texture.cpp sdltoprojectM.h video_init.cpp ConfigFile.h ConfigFile.cpp)

INCLUDE(FindPkgConfig.cmake)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(SDL)

pkg_search_module(PROJECTM REQUIRED libprojectM)

ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='"${PROJECTM_PREFIX}"')

INCLUDE_DIRECTORIES(${PROJECTM_INCLUDEDIR} ${SDL_INCLUDE_DIR} ${GTK_INCLUDE_DIR})
LINK_DIRECTORIES(${PROJECTM_LDFLAGS})

TARGET_LINK_LIBRARIES(projectM-test projectM  ${SDL_LIBRARY} ${GTK_LIBRARIES})
TARGET_LINK_LIBRARIES(projectM-test-texture projectM  ${SDL_LIBRARY} ${GTK_LIBRARIES})

#INSTALL(TARGETS projectM-test DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/xmms/Visualization )