AM_CPPFLAGS = \
${my_CFLAGS} \
-include $(top_builddir)/config.h \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-DPROJECTM_PREFIX=\""${prefix}"\" \
-DRESOURCE_PREFIX=\""share/projectM"\" \
-I${top_srcdir}/src/libprojectM \
-I${top_srcdir}/src/libprojectM/Renderer \
-I${top_srcdir}/src/projectM-qt \
${jack_CFLAGS} \
${qt_CPPFLAGS} \
-fPIC \
${qt_CFLAGS}

# TODO: add remote control if we have qxt network lib
# if QT_NETWORK_ENABLED
# EXTRA_SRCS = HTTPRemoteControl.cpp
# endif


# TODO: build qprojectM-jack and projectM-jack as separate programs

bin_PROGRAMS = projectM_jack
projectM_jack_SOURCES = qprojectM-jack.cpp ConfigFile.cpp ${EXTRA_SRCS}
projectM_jack_LDADD = \
  ../projectM-qt/libprojectM_qt.a \
  ${jack_LIBS} \
  ${qt_LIBS} \
  ../libprojectM/libprojectM.la \
  -lSDL
projectM_jack_LDFLAGS = -static -fPIC ${qt_LDFLAGS}
projectM_jack_PROGRAM = projectM-jack

dist_man_MANS = projectM-jack.1
