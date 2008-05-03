PROJECT(projectM-jack)

OPTION(QT_JACK_SUPPORT "Enable Qt4 GUI Support for the projectM jack port" ON)
OPTION(BUILD_PROJECTM_JACK_STATIC "Build a binary that only depends on jack and stnadard MACOSX libraries" OFF)

if (QT_JACK_SUPPORT)

set(QT_USE_OPENGL TRUE)

include(${QT_USE_FILE} FindPkgConfig.cmake)

if (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
	pkg_search_module(LIBPROJECTM-QT REQUIRED libprojectM-qt)
	ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='"${LIBPROJECTM_PREFIX}"')
else(${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	set(LIBPROJECTM_FOUND true)
	set(LIBPROJECTM-QT_FOUND true)
	ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='"${CMAKE_INSTALL_PREFIX}"')
endif(${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(Qt4 REQUIRED)

pkg_search_module(JACK REQUIRED jack)

if (LIBPROJECTM_FOUND)
MESSAGE (STATUS "[projectM-jack] projectM detected.")
else(LIBPROJECTM_FOUND)
MESSAGE (FATAL_ERROR "projectM NOT detected. Please install the projectM module or build from the top level projectM source directory.")
endif(LIBPROJECTM_FOUND)

if (LIBPROJECTM-QT_FOUND)
MESSAGE (STATUS "[projectM-jack] projectM-qt detected.")
else(LIBPROJECTM-QT_FOUND)
MESSAGE (FATAL_ERROR "projectM-qt NOT detected. Please install the projectM-qt module or build from the top level projectM source directory. You can also disable QT_PROJECTM_JACK support and use the SDL port instead")
endif(LIBPROJECTM-QT_FOUND)

if (JACK_FOUND)
MESSAGE(STATUS "[projectM-jack] jack detected.")
else (JACK_FOUND)
MESSAGE( "JACK is NOT found. Please install jack from http://www.jackaudio.org.")
endif(JACK_FOUND)


set(projectM_jack_SRCS
  qprojectM-jack.cpp ConfigFile.h ConfigFile.cpp
)

set(projectM_jack_MOC_HDRS

)


# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
qt4_wrap_cpp(projectM_jack_MOC_SRCS ${projectM_MOC_HDRS})


ADD_DEFINITIONS(-DLINUX)
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_NO_DEBUG)


if (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")
   set(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/projectM-engine ${PROJECTM_ROOT_SOURCE_DIR}/projectM-qt)
   set(PROJECTM_LINK ${PROJECTM_ROOT_BINARY_DIR}/projectM-engine ${PROJECTM_ROOT_BINARY_DIR}/projectM-qt)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
   set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libprojectM-qt ${LIBPROJECTM-QT_INCLUDEDIR}/libprojectM)
   set(PROJECTM_LINK ${LIBPROJECTM_LDFLAGS} ${LIBPROJECTM-QT_LDFLAGS})
# projectM-qt places itself in same location as (lib)projectM 
# whether it should is a whole other issue
endif (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")


INCLUDE_DIRECTORIES(${PROJECTM_INCLUDE} ${JACK_INCLUDEDIR}  ${SDL_INCLUDE_DIR} ${GTK_INCLUDE_DIR} ${QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR})

LINK_DIRECTORIES(${PROJECTM_LINK} ${JACK_LDFLAGS})

ADD_EXECUTABLE(projectM-jack ${projectM_jack_SRCS} ${projectM_jack_MOC_SRCS})

if (BUILD_PROJECTM_JACK_STATIC)
SET(PACKAGED_LIBRARIES ${PROJECTM_ROOT_BINARY_DIR}/projectM-engine/libprojectM.a ${PROJECTM_ROOT_BINARY_DIR}/projectM-qt/libprojectM-qt.a)
else (BUILD_PROJECTM_JACK_STATIC)
SET(PACKAGED_LIBRARIES projectM-qt projectM)
endif (BUILD_PROJECTM_JACK_STATIC)

TARGET_LINK_LIBRARIES(projectM-jack ${PACKAGED_LIBRARIES} jack ${QT_QTGUI_LIBRARIES} ${QT_QTOPENGL_LIBRARIES} ${QT_QTCORE_LIBRARIES} ${QT_LIBRARIES})

INSTALL(TARGETS projectM-jack DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
install(FILES "projectM-jack.desktop" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)

else (QT_JACK_SUPPORT)
ADD_EXECUTABLE(projectM-jack projectM-jack.cpp sdltoprojectM.h video_init.cpp ConfigFile.h ConfigFile.cpp)

INCLUDE(FindPkgConfig.cmake)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(SDL)

if (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
else (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	set(LIBPROJECTM_FOUND true)
endif(${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})

pkg_search_module(JACK REQUIRED jack)

if (JACK_FOUND)
MESSAGE(STATUS "[projectM-jack] jack detected.")
else (JACK_FOUND)
MESSAGE(FATAL_ERROR "JACK is NOT found. Please install jack from http://www.jackaudio.org.")
endif(JACK_FOUND)

if (LIBPROJECTM_FOUND)
MESSAGE (STATUS "[projectM-jack] projectM detected.")
else(LIBPROJECTM_FOUND)
MESSAGE (FATAL_ERROR "projectM NOT detected. Please install the projectM module or build from the top level projectM source directory.")
endif(LIBPROJECTM_FOUND)

ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='"${PROJECTM_PREFIX}"')

if (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")
   set(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/projectM-engine)
   set(PROJECTM_LINK ${PROJECTM_ROOT_BINARY_DIR}/projectM-engine)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
   set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libprojectM)
   set(PROJECTM_LINK ${LIBPROJECTM_LDFLAGS})
endif (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")

INCLUDE_DIRECTORIES(${PROJECTM_INCLUDE} ${JACK_INCLUDEDIR}  ${SDL_INCLUDE_DIR} ${GTK_INCLUDE_DIR})
LINK_DIRECTORIES(${PROJECTM_LINK} ${JACK_LDFLAGS})

TARGET_LINK_LIBRARIES(projectM-jack projectM jack  ${SDL_LIBRARY} ${GTK_LIBRARIES})

INSTALL(TARGETS projectM-jack DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
install(FILES "projectM-jack.desktop" DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
endif(QT_JACK_SUPPORT)

