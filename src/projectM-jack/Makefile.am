bin_PROGRAMS = jack-projectM

AM_CXXFLAGS = -Wall  -I$(top_builddir) -I$(top_srcdir) @SDL_CFLAGS@ @JACK_CFLAGS@ @LIBPROJECTM_CFLAGS@ -DLINUX
LIBS = @SDL_LIBS@ @JACK_LIBS@ @LIBPROJECTM_LIBS@ -lprojectM -lGL -lGLU
jack_projectM_SOURCES =  video_init.cpp jack-projectM.cpp video_init.h sdltoprojectm.h ConfigFile.cpp ConfigFile.h
