AM_CPPFLAGS = \
${my_CFLAGS} \
-include $(top_builddir)/config.h \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-I${top_srcdir}/src/libprojectM \
-I${top_srcdir}/src/libprojectM/Renderer \
${SDL_CFLAGS} \
-I$(top_srcdir)/src/libprojectM/Renderer/imgui

bin_PROGRAMS = projectMSDL
projectMSDL_SOURCES = pmSDL.cpp projectM_SDL_main.cpp pmSDL.hpp imgui_impl_sdl.cpp
projectMSDL_LDADD =
#projectMSDL_LDADD += -lasan
projectMSDL_LDADD += ${SDL_LIBS}	../libprojectM/libprojectM.la
projectMSDL_LDFLAGS = -static
projectMSDL_PROGRAM = projectMSDL
