PROJECT(qprojectM-jack)

set(QT_USE_OPENGL TRUE)

include(${QT_USE_FILE} FindPkgConfig.cmake)

FIND_PACKAGE(OpenGL)
FIND_PACKAGE(Qt4 REQUIRED)

# WRONG! How do I do this? using FIND_PACKAGE?
##pkg_search_module(PROJECTM REQUIRED projectM qprojectM)

pkg_search_module(JACK REQUIRED jack)
pkg_search_module(PROJECTM REQUIRED libprojectM)
pkg_search_module(PROJECTM REQUIRED libqprojectM)

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

INCLUDE_DIRECTORIES(${PROJECTM_ROOT_SOURCE_DIR}/projectM-engine ${PROJECTM_ROOT_SOURCE_DIR}/qprojectM ${JACK_INCLUDEDIR}  ${SDL_INCLUDE_DIR} ${GTK_INCLUDE_DIR} ${QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR})

LINK_DIRECTORIES(${PROJECTM_ROOT_BINARY_DIR}/projectM-engine ${PROJECTM_ROOT_BINARY_DIR}/qprojectM ${JACK_LDFLAGS})

ADD_EXECUTABLE(qprojectM-jack ${qprojectM_jack_SRCS} ${qprojectM_jack_MOC_SRCS})

TARGET_LINK_LIBRARIES(qprojectM-jack qprojectM projectM jack ${QT_QTGUI_LIBRARIES} ${QT_QTOPENGL_LIBRARIES} ${QT_QTCORE_LIBRARIES} ${QT_LIBRARIES})

INSTALL(TARGETS qprojectM-jack DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )