AM_CPPFLAGS = \
-include $(top_builddir)/config.h \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-I${top_srcdir}/src/libprojectM \
-I${top_srcdir}/src/libprojectM/Renderer \
$SDL_CFLAGS

AM_CFLAGS = ${my_CFLAGS} \
-fvisibility=hidden \
-ffunction-sections \
-fdata-sections

bin_PROGRAMS = projectMSDL
projectMSDL_SOURCES = pmSDL.cpp projectM_SDL_main.cpp
projectMSDL_LDADD = -libprojectM $SDL_LIBS
projectMSDL_PROGRAM = projectMSDL
