AM_CPPFLAGS = \
${my_CFLAGS} \
-include $(top_builddir)/config.h \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-DPROJECTM_PREFIX=\""${prefix}"\" \
-I${top_srcdir}/src/libprojectM \
-I${top_srcdir}/src/libprojectM/Renderer \
-I${top_srcdir}/src/projectM-qt \
${jack_CFLAGS} \
${qt_CPPFLAGS} \
${qt_CFLAGS}

# if QT_NETWORK_ENABLED
# EXTRA_SRCS = HTTPRemoteControl.cpp
# endif

bin_PROGRAMS = projectM_jack
projectM_jack_SOURCES = projectM-jack.cpp qprojectM-jack.cpp video_init.cpp ConfigFile.cpp ${EXTRA_SRCS}
projectM_jack_LDADD =
projectM_jack_LDADD += ${jack_LIBS} ${qt_LDFLAGS} ../libprojectM/libprojectM.la
projectM_jack_LDFLAGS = -static
projectM_jack_PROGRAM = projectM-jack

dist_man_MANS = projectM-jack.1
