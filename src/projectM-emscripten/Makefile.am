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

bin_PROGRAMS = projectW.bc
projectW_bc_SOURCES = ../libprojectM/KeyHandler.cpp projectM_SDL_emscripten.cpp
projectW_bc_LDADD = ${SDL_LIBS}	${top_srcdir}/src/libprojectM/libprojectM.la
projectW_bc_LDFLAGS = $(EMSCRIPTEN_FLAGS) -static
projectW_bc_PROGRAM = projectW.bc

generate-html:
	emcc -s ALLOW_MEMORY_GROWTH=1 $(EMSCRIPTEN_FLAGS) projectW.bc -o projectW.html

projectW.html: generate-html

run: projectW.bc projectW.html
	emrun projectW.html
