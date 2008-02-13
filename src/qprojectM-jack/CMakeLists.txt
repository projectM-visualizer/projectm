PROJECT(qprojectM-jack)

set(QT_USE_OPENGL TRUE)

include(${QT_USE_FILE} FindPkgConfig.cmake)

if (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
	pkg_search_module(LIBQPROJECTM REQUIRED libqprojectM)
else(${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
	set(LIBPROJECTM_FOUND true)
	set(LIBQPROJECTM_FOUND true)
endif(${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(Qt4 REQUIRED)

pkg_search_module(JACK REQUIRED jack)

if (LIBPROJECTM_FOUND)
MESSAGE (STATUS "projectM detected.")
else(LIBPROJECTM_FOUND)
MESSAGE (FATAL_ERROR "projectM NOT detected. Please install the projectM module or build from the top level projectM source directory.")
endif(LIBPROJECTM_FOUND)

if (LIBQPROJECTM_FOUND)
MESSAGE (STATUS "qprojectM detected.")
else(LIBQPROJECTM_FOUND)
MESSAGE (FATAL_ERROR "qprojectM NOT detected. Please install the qprojectM module or build from the top level projectM source directory.")
endif(LIBQPROJECTM_FOUND)

if (JACK_FOUND)
MESSAGE(STATUS "Jack detected")
else (JACK_FOUND)
MESSAGE(FATAL_ERROR "JACK is NOT found. Please install jack from http://www.jackaudio.org.")
endif(JACK_FOUND)

# the variable "qprojectM_SRCS" contains all .cpp files of this project
set(qprojectM_jack_SRCS
  qprojectM-jack.cpp ConfigFile.h ConfigFile.cpp
)

set(qprojectM_jack_MOC_HDRS

)


# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
qt4_wrap_cpp(qprojectM_jack_MOC_SRCS ${qprojectM_MOC_HDRS})


ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='"${PROJECTM_PREFIX}"')
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_NO_DEBUG)


if (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")
   set(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/projectM-engine ${PROJECTM_ROOT_SOURCE_DIR}/qprojectM)
   set(PROJECTM_LINK ${PROJECTM_ROOT_BINARY_DIR}/projectM-engine ${PROJECTM_ROOT_BINARY_DIR}/qprojectM)
elseif (${CMAKE_PROJECT_NAME} MATCHES ${PROJECT_NAME})
   set(PROJECTM_INCLUDE ${LIBPROJECTM_INCLUDEDIR}/libqprojectM ${LIBQPROJECTM_INCLUDEDIR}/libprojectM)
   set(PROJECTM_LINK ${LIBPROJECTM_LDFLAGS} ${LIBQPROJECTM_LDFLAGS})
   # qprojectM places itself in same location as (lib)projectM 
   # whether it should is a whole other issue
endif (${CMAKE_PROJECT_NAME} MATCHES "PROJECTM_ROOT")


INCLUDE_DIRECTORIES(${PROJECTM_INCLUDE} ${JACK_INCLUDEDIR}  ${SDL_INCLUDE_DIR} ${GTK_INCLUDE_DIR} ${QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR})

LINK_DIRECTORIES(${PROJECTM_LINK} ${JACK_LDFLAGS})

ADD_EXECUTABLE(qprojectM-jack ${qprojectM_jack_SRCS} ${qprojectM_jack_MOC_SRCS})

TARGET_LINK_LIBRARIES(qprojectM-jack qprojectM projectM jack ${QT_QTGUI_LIBRARIES} ${QT_QTOPENGL_LIBRARIES} ${QT_QTCORE_LIBRARIES} ${QT_LIBRARIES})

INSTALL(TARGETS qprojectM-jack DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )