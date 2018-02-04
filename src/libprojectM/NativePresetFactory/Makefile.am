lib_LTLIBRARIES=libNativePresetFactory.la

libNativePresetFactory_la_SOURCES= \
	NativePreset.hpp NativePresetFactory.hpp \
  MilkdropCompatability.hpp NativePresetFactory.cpp

libNativePresetFactory_la_CPPFLAGS = ${my_CFLAGS} \
  -I$(top_srcdir)/src/libprojectM

libNativePresetFactory_la_LDFLAGS = -static
