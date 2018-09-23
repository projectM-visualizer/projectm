noinst_LTLIBRARIES=libNativePresetFactory.la

libNativePresetFactory_la_SOURCES= \
	NativePreset.hpp NativePresetFactory.hpp \
  MilkdropCompatability.hpp NativePresetFactory.cpp \
  MilkdropCompatability.hpp NativePreset.hpp NativePresetFactory.hpp

libNativePresetFactory_la_CPPFLAGS = ${my_CFLAGS} \
  -include $(top_builddir)/config.h \
  -I$(top_srcdir)/src/libprojectM
