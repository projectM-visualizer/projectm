AM_CPPFLAGS = \
${my_CFLAGS} \
-include $(top_builddir)/config.h \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-I${top_srcdir}/src/libprojectM \
-I${top_srcdir}/src/libprojectM/Renderer \
${SDL_CFLAGS}

REALSRCDIR=${top_srcdir}/src/projectM-sdl

EMSCRIPTEN_FLAGS =  -s USE_SDL=2 -s USE_WEBGL2=1 -s WASM=0

bin_PROGRAMS = projectW.bc
projectW_bc_SOURCES = $(REALSRCDIR)/pmSDL.cpp $(REALSRCDIR)/projectM_SDL_main.cpp $(REALSRCDIR)/pmSDL.hpp
projectW_bc_LDADD = ${SDL_LIBS}	${top_srcdir}/src/libprojectM/libprojectM.la
projectW_bc_LDFLAGS = $(EMSCRIPTEN_FLAGS)
projectW_bc_PROGRAM = projectW.bc

projectW.html: generate-html

generate-html:
	emcc  ALLOW_MEMORY_GROWTH=1 $(EMSCRIPTEN_FLAGS) projectW.bc

run: projectW.html
	emrun projectW.html
