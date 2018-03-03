EXTRA_DIST=
CLEANFILES=

SUBDIRS=Renderer NativePresetFactory MilkdropPresetFactory

AM_CPPFLAGS = \
-include $(top_builddir)/config.h \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-I$(top_srcdir)/src/libprojectM \
-I$(top_srcdir)/src/libprojectM/Renderer

# system headers/libraries to install
include_HEADERS = projectM.hpp
lib_LTLIBRARIES = libprojectM.la  # public, possibly-shared library
# libprojectM_la_LDFLAGS = -no-undefined -version-info 0:2:0
libprojectM_la_LDFLAGS = -version-info 0:2:0

# link libRenderer, MilkdropPresetFactory, NativePresetFactory, and libprojectM sources
libprojectM_la_LIBADD = \
$(top_srcdir)/src/libprojectM/MilkdropPresetFactory/libMilkdropPresetFactory.la \
$(top_srcdir)/src/libprojectM/NativePresetFactory/libNativePresetFactory.la \
$(top_srcdir)/src/libprojectM/Renderer/libRenderer.la
libprojectM_la_SOURCES = ConfigFile.cpp Preset.cpp PresetLoader.cpp timer.cpp \
  KeyHandler.cpp PresetChooser.cpp TimeKeeper.cpp PCM.cpp PresetFactory.cpp \
	fftsg.cpp wipemalloc.cpp PipelineMerger.cpp PresetFactoryManager.cpp projectM.cpp

pkginclude_HEADERS = projectM.hpp
pkgconfigdir = $(libdir)/pkgconfig
# pkgconfig_DATA = src/libprojectM.pc
# EXTRA_DIST += src/libprojectM.pc.in
# CLEANFILES += src/libprojectM.pc
