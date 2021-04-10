AM_CPPFLAGS = \
${my_CFLAGS} \
-I$(top_builddir)/config.h \
-I$(top_builddir)/vendor \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-I${top_srcdir}/src/libprojectM \
-I${top_srcdir}/src/libprojectM/Renderer \
${SDL_CFLAGS}

EMSCRIPTEN_FLAGS = -g
# -s WASM=0
# -O2

# lib_LTLIBRARIES = libprojectW.la
# libprojectW_la_SOURCES = ../libprojectM/KeyHandler.cpp projectM_SDL_emscripten.cpp
# libprojectW_la_LIBADD = ${SDL_LIBS}	${top_srcdir}/src/libprojectM/libprojectM.la
# libprojectW_la_LDFLAGS = $(EMSCRIPTEN_FLAGS) -static
# projectW_la_PROGRAM = projectW.js

projectW.js:
	emcc -s ALLOW_MEMORY_GROWTH=1 $(EMSCRIPTEN_FLAGS) $(AM_CPPFLAGS)	${top_srcdir}/src/libprojectM/libprojectM.la ../libprojectM/KeyHandler.cpp projectM_SDL_emscripten.cpp -o projectW.js

generate-html: projectW.js
	emcc projectW.js -o projectW.html

projectW.html: generate-html

run: projectW.html
	emrun projectW.html
