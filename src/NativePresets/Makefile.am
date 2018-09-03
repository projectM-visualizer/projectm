AM_CPPFLAGS = \
-include $(top_builddir)/config.h \
-I${top_srcdir}/src/libprojectM \
-I${top_srcdir}/src/libprojectM/Renderer \
-I${top_srcdir}/src/libprojectM/NativePresetFactory

AM_CFLAGS = ${my_CFLAGS} \
-fvisibility=hidden \
-ffunction-sections \
-fdata-sections

presetsdir = $(libdir)/projectM

presets_LTLIBRARIES = \
  libMstressJuppyDancer.la \
  libRLGFractalDrop7c.la \
	libRovastarDarkSecret.la \
  libRovastarDriftingChaos.la \
  libRovastarFractalSpiral.la \
  libRovastarFractopiaFrantic.la

libMstressJuppyDancer_la_SOURCES = MstressJuppyDancer.cpp
libRLGFractalDrop7c_la_SOURCES = RLGFractalDrop7c.cpp
libRovastarDarkSecret_la_SOURCES = RovastarDarkSecret.cpp
libRovastarDriftingChaos_la_SOURCES = RovastarDriftingChaos.cpp
libRovastarFractalSpiral_la_SOURCES = RovastarFractalSpiral.cpp
libRovastarFractopiaFrantic_la_SOURCES = RovastarFractopiaFrantic.cpp

libMstressJuppyDancer_la_LDFLAGS = -avoid-version
libRLGFractalDrop7c_la_LDFLAGS = -avoid-version
libRovastarDarkSecret_la_LDFLAGS = -avoid-version
libRovastarDriftingChaos_la_LDFLAGS = -avoid-version
libRovastarFractalSpiral_la_LDFLAGS = -avoid-version
libRovastarFractopiaFrantic_la_LDFLAGS = -avoid-version
