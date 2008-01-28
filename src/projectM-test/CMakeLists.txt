PROJECT(projectM-tests)

ADD_EXECUTABLE(projectM-test projectM-test.cpp sdltoprojectM.h video_init.cpp ConfigFile.h ConfigFile.cpp getConfigFilename.cpp getConfigFilename.h)
ADD_EXECUTABLE(projectM-test-memleak projectM-test-memleak.cpp sdltoprojectM.h video_init.cpp ConfigFile.h ConfigFile.cpp)

ADD_EXECUTABLE(projectM-test-texture projectM-test-texture.cpp sdltoprojectM.h video_init.cpp ConfigFile.h ConfigFile.cpp)

INCLUDE(FindPkgConfig.cmake)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(SDL)

pkg_search_module(PROJECTM REQUIRED libprojectM)

ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='"${PROJECTM_PREFIX}"')

INCLUDE_DIRECTORIES(${PROJECTM_ROOT_SOURCE_DIR}/projectM-engine ${SDL_INCLUDE_DIR} ${GTK_INCLUDE_DIR})
LINK_DIRECTORIES(${PROJECTM_ROOT_SOURCE_DIR}/projectM-engine)

TARGET_LINK_LIBRARIES(projectM-test projectM  ${SDL_LIBRARY} ${GTK_LIBRARIES})
TARGET_LINK_LIBRARIES(projectM-test-memleak projectM  ${SDL_LIBRARY} ${GTK_LIBRARIES})
TARGET_LINK_LIBRARIES(projectM-test-texture projectM  ${SDL_LIBRARY} ${GTK_LIBRARIES})

INSTALL(TARGETS projectM-test projectM-test-texture projectM-test-memleak DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
