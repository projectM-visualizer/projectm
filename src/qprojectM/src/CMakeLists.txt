# the next line sets up include and link directories and defines some variables that we will use.
# you can modify the behavior by setting some variables, e.g.
set(QT_USE_OPENGL TRUE)
# -> this will cause cmake to include and link against the OpenGL module
include(${QT_USE_FILE} FindPkgConfig.cmake)


ADD_DEFINITIONS(-DLINUX -DPROJECTM_PREFIX='"${PROJECTM_PREFIX}"')
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

# the variable "qprojectM_SRCS" contains all .cpp files of this project
set(qprojectM_SRCS
    QProjectM_MainWindow.cpp
    QProjectM_MainWindow.hpp
    ConfigFile.h ConfigFile.cpp
    QProjectMFileDialog.hpp
    QPlaylistModel.cpp
    QPlaylistModel.hpp
)


set(qprojectM_MOC_HDRS
    QProjectM_MainWindow.hpp
    QProjectMFileDialog.hpp
    QPlaylistModel.hpp
)

set(qprojectM_UIS
    QProjectM_MainWindow.ui
)

# Qt resource file
set( qprojectM_RESOURCES
    application.qrc
)

# compile resources
qt4_add_resources( qprojectM_RC_SRCS ${qprojectM_RESOURCES} )

qt4_wrap_ui(qprojectM_UIS_H ${qprojectM_UIS})

# After this call, foo_MOC_SRCS = moc_Class1.cxx moc_Class2.cxx moc_Class3.cxx.
qt4_wrap_cpp(qprojectM_MOC_SRCS ${qprojectM_MOC_HDRS})

ADD_LIBRARY(qprojectM SHARED ${qprojectM_SRCS} ${qprojectM_RC_SRCS} ${qprojectM_MOC_SRCS} ${qprojectM_UIS_H})
LINK_DIRECTORIES(${PROJECTM_LDFLAGS})

include_directories(${QT_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR} ${CMAKE_CURRENT_LIBRARY_DIR})

# link the "qprojectM" target against the Qt libraries. which libraries exactly, is defined by the "include(${QT_USE_FILE})" line above, which sets up this variable.
target_link_libraries(qprojectM projectM ${QT_QTGUI_LIBRARIES} ${QT_QTOPENGL_LIBRARIES} ${QT_LIBRARIES})

install(FILES QProjectM_MainWindow.hpp ui_QProjectM_MainWindow.h DESTINATION include/libprojectM)
install(TARGETS qprojectM DESTINATION lib)
