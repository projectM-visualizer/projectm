AM_CPPFLAGS = \
${my_CFLAGS} \
-include $(top_builddir)/config.h \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-I${top_srcdir}/src/libprojectM \
-I${top_srcdir}/src/libprojectM/Renderer

bin_PROGRAMS = h2g
h2g_SOURCES = h2g_main.cpp
h2g_LDADD =
h2g_LDADD += ../libprojectM/libprojectM.la
h2g_LDFLAGS = -static
h2g_PROGRAM = h2g