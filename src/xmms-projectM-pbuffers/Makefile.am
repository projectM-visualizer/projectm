lib_LTLIBRARIES = libprojectm_xmms.la

libdir = @XMMS_VISUALIZATION_PLUGIN_DIR@

AM_CFLAGS = -Wall -DLINUX @XMMS_CFLAGS@ -I$(top_builddir) -I$(top_srcdir) 
LIBS = @XMMS_LIBS@ @GL_LIBS@ @FTGL_LIBS@ -lprojectM -lGL -lGLU
libprojectm_xmms_la_LDFLAGS = -module -avoid-version 
libprojectm_xmms_la_SOURCES =  main.c xtoprojectm.h
