AM_CPPFLAGS = \
${my_CFLAGS} \
-include $(top_builddir)/config.h \
-I$(top_builddir)/vendor \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-I${top_srcdir}/src/libprojectM \
-I${top_srcdir}/src/libprojectM/Renderer \
${SDL_CFLAGS}

EMSCRIPTEN_FLAGS =  -s USE_SDL=2 -s USE_WEBGL2=1 
# -s WASM=0

bin_PROGRAMS = projectW.bc
projectW_bc_SOURCES = ../libprojectM/KeyHandler.cpp projectM_SDL_emscripten.cpp
projectW_bc_LDADD = ${SDL_LIBS}	${top_srcdir}/src/libprojectM/libprojectM.la
projectW_bc_LDFLAGS = $(EMSCRIPTEN_FLAGS) -static
projectW_bc_PROGRAM = projectW.bc

projectW.html: generate-html

generate-html:
	emcc -s ALLOW_MEMORY_GROWTH=1 $(EMSCRIPTEN_FLAGS) projectW.bc -o projectW.html

run: projectW.html
	emrun --browser chrome projectW.html
