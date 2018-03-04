EXTRA_DIST=
CLEANFILES=

SUBDIRS=Renderer NativePresetFactory MilkdropPresetFactory

AM_CPPFLAGS = \
-include $(top_builddir)/config.h \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-I$(top_srcdir)/src/libprojectM \
-I$(top_srcdir)/src/libprojectM/Renderer

# system headers/libraries/data to install
include_HEADERS = projectM.hpp
lib_LTLIBRARIES = libprojectM.la  # public, possibly-shared library

# link flags
libprojectM_la_LDFLAGS = -no-undefined -version-info 0:2:0

# link libRenderer, MilkdropPresetFactory, NativePresetFactory, and libprojectM sources
libprojectM_la_LIBADD = \
$(top_srcdir)/src/libprojectM/MilkdropPresetFactory/libMilkdropPresetFactory.la \
$(top_srcdir)/src/libprojectM/NativePresetFactory/libNativePresetFactory.la \
$(top_srcdir)/src/libprojectM/Renderer/libRenderer.la
libprojectM_la_SOURCES = ConfigFile.cpp Preset.cpp PresetLoader.cpp timer.cpp \
  KeyHandler.cpp PresetChooser.cpp TimeKeeper.cpp PCM.cpp PresetFactory.cpp \
	fftsg.cpp wipemalloc.cpp PipelineMerger.cpp PresetFactoryManager.cpp projectM.cpp \
  Common.hpp                 PipelineMerger.hpp         PresetLoader.hpp\
	HungarianMethod.hpp        Preset.hpp                 RandomNumberGenerators.hpp\
	IdleTextures.hpp           PresetChooser.hpp          TimeKeeper.hpp\
	KeyHandler.hpp             PresetFactory.hpp          projectM.hpp\
	PCM.hpp                    PresetFactoryManager.hpp\
	projectM.hpp \
	src/libprojectM/ConfigFile.h       src/libprojectM/glew.h\
	src/libprojectM/carbontoprojectM.h src/libprojectM/glxew.h\
	src/libprojectM/cocoatoprojectM.h  src/libprojectM/lvtoprojectM.h\
	src/libprojectM/dlldefs.h          src/libprojectM/resource.h\
	src/libprojectM/event.h            src/libprojectM/sdltoprojectM.h\
	src/libprojectM/fatal.h            src/libprojectM/timer.h\
	src/libprojectM/fftsg.h            src/libprojectM/win32-dirent.h\
	src/libprojectM/glError.h          src/libprojectM/wipemalloc.h


pkgconfigdir = $(libdir)/pkgconfig
# pkgconfig_DATA = src/libprojectM.pc
# EXTRA_DIST += src/libprojectM.pc.in
# CLEANFILES += src/libprojectM.pc
