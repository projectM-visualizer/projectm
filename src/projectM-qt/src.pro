SOURCES += qgltest.cpp \
           main.cpp
HEADERS += qgltest.h \
 QProjectMFileDialog.hpp
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt
TARGET = ../bin/qgltest
RESOURCES = application.qrc
DISTFILES += ../CMakeLists.txt \
CMakeLists.txt
