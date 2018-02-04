EXTRA_DIST=
CLEANFILES=

SUBDIRS=Renderer

AM_CPPFLAGS = \
-include $(top_builddir)/config.h \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-I$(top_srcdir)/src/libprojectM \
-I$(top_srcdir)/src/libprojectM/Renderer

AM_CFLAGS = ${my_CFLAGS} \
-fvisibility=hidden \
-ffunction-sections \
-fdata-sections

AM_LDFLAGS = \
-Wl,--gc-sections \
-Wl,--as-needed

lib_LTLIBRARIES = libprojectM.la

libprojectM_la_SOURCES = ConfigFile.cpp Preset.cpp PresetLoader.cpp timer.cpp \
  KeyHandler.cpp PresetChooser.cpp TimeKeeper.cpp PCM.cpp PresetFactory.cpp \
	fftsg.cpp wipemalloc.cpp PipelineMerger.cpp PresetFactoryManager.cpp projectM.cpp

libprojectM_la_LIBADD = -lz Renderer/libRenderer.la
libprojectM_la_LDFLAGS = -export-symbols-regex '^projectM_[^_]'

pkginclude_HEADERS = projectM.hpp
pkgconfigdir = $(libdir)/pkgconfig
# pkgconfig_DATA = src/libprojectM.pc
# EXTRA_DIST += src/libprojectM.pc.in
# CLEANFILES += src/libprojectM.pc
