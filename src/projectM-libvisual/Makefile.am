## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(PROJECTM)

EXTRA_LTLIBRARIES = actor_projectM.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS = -L. -L$(prefix)/lib  @LIBVISUAL_LIBS@ -lprojectM -lGL -lGLU

AM_CXXFLAGS = @LIBVISUAL_CFLAGS@ -DLINUX

INCLUDES = $(all_includes) \
	-I$(top_srcdir) -I/usr/include

actor_projectM_la_LDFLAGS =  -rpath $(libdir) -module -avoid-version 

actor_projectM_la_SOURCES = actor_projectM.cpp

