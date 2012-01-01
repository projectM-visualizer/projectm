PROJECT(projectM-libvisual-alsa)
cmake_minimum_required(VERSION 2.4.0)

ADD_EXECUTABLE(projectM-libvisual-alsa projectM-libvisual-alsa.c display.c glxdriver.c x11driver.c sdldriver.c lv_x11_key.c)

cmake_policy(SET CMP0005 OLD)

INCLUDE(FindPkgConfig.cmake)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(SDL)
FIND_PACKAGE(X11)
INCLUDE(cmake/CPack-projectM.cmake)

if (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
else (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	set(LIBPROJECTM_FOUND true)
endif(${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})

if (LIBPROJECTM_FOUND)
MESSAGE (STATUS "[projectM-libvisual-alsa] projectM detected.")
else(LIBPROJECTM_FOUND)
MESSAGE (FATAL_ERROR "projectM NOT detected. Please install the projectM module or build from the top level projectM source directory.")
endif(LIBPROJECTM_FOUND)

pkg_search_module(LIBVISUAL REQUIRED libvisual-0.4)

if(LIBVISUAL_FOUND)
MESSAGE(STATUS "[projectM-libvisual] libvisual detected.")
else(LIBVISUAL_FOUND)
MESSAGE(FATAL_ERROR "libvisual 0.4 not found! Please visit http://libvisual.sf.net and download the module.")
endif(LIBVISUAL_FOUND)

ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='\"${PROJECTM_PREFIX}\"')

INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR} ${LIBVISUAL_INCLUDEDIR} ${X11_INCLUDE_DIR})

LINK_DIRECTORIES(${LIBVISUAL_LDFLAGS})

TARGET_LINK_LIBRARIES(projectM-libvisual-alsa -lXxf86vm -L/usr/X11R6/lib ${OPENGL_LIBRARIES} ${LIBVISUAL_LIBRARIES} ${X11_LIBRARIES} ${SDL_LIBRARY} ${GTK_LIBRARIES})

INSTALL(TARGETS projectM-libvisual-alsa DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
install(FILES "projectM-libvisual-alsa.desktop" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)

