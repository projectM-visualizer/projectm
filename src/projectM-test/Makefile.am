AM_CPPFLAGS = \
${my_CFLAGS} \
-include $(top_builddir)/config.h \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-I${top_srcdir}/src/libprojectM \
-I${top_srcdir}/src/libprojectM/Renderer \
${SDL_CFLAGS}

# TODO investigate these old tests programms projectM_test projectM_test-memleak projectM_test-texture
bin_PROGRAMS = projectM-unittest

projectM_unittest_SOURCES = projectM-unittest.cpp getConfigFilename.cpp ConfigFile.h getConfigFilename.h
projectM_unittest_LDADD =
projectM_unittest_LDADD += ${SDL_LIBS}	../libprojectM/libprojectM.la
projectM_unittest_LDFLAGS = -static
projectM_unittest_PROGRAM = projectM-unittest
