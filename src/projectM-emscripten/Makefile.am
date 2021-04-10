AM_CPPFLAGS = \
${my_CFLAGS} \
-I$(top_builddir)/config.h \
-I$(top_builddir)/vendor \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-I${top_srcdir}/src/libprojectM \
-I${top_srcdir}/src/libprojectM/Renderer \
${SDL_CFLAGS}

EMSCRIPTEN_FLAGS = $(AM_CPPFLAGS) -g -s USE_SDL=2 -s USE_WEBGL2
# -s WASM=0
# -O2

# lib_LTLIBRARIES = libprojectW.la
# libprojectW_la_SOURCES = ../libprojectM/KeyHandler.cpp projectM_SDL_emscripten.cpp
# libprojectW_la_LIBADD = ${SDL_LIBS}	${top_srcdir}/src/libprojectM/libprojectM.la
# libprojectW_la_LDFLAGS = $(EMSCRIPTEN_FLAGS) -static
# projectW_la_PROGRAM = projectW.js

projectW.html:
	emcc -s ALLOW_MEMORY_GROWTH=1 $(EMSCRIPTEN_FLAGS) ${top_srcdir}/src/libprojectM/.libs/libprojectM.a ../libprojectM/KeyHandler.cpp projectM_SDL_emscripten.cpp -o projectW.html

run: projectW.html
	emrun projectW.html
