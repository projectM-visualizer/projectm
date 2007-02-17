lib_LTLIBRARIES = libprojectm_xmms.la

libdir = @XMMS_VISUALIZATION_PLUGIN_DIR@

AM_CFLAGS = -Wall @XMMS_CFLAGS@ -I$(top_builddir) -I$(top_srcdir) @SDL_CFLAGS@
LIBS = @XMMS_LIBS@ @SDL_LIBS@ @GL_LIBS@ @FTGL_LIBS@ -lprojectM -lGL -lGLU
libprojectm_xmms_la_LDFLAGS = -module -avoid-version 
libprojectm_xmms_la_SOURCES =  main.c video_init.h video_init.c sdltoprojectm.h
