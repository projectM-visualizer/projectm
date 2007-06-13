lib_LTLIBRARIES = libprojectm_xmms.la

libdir = @XMMS_VISUALIZATION_PLUGIN_DIR@

AM_CFLAGS = -Wall @XMMS_CFLAGS@ -I$(top_builddir) -I$(top_srcdir) @GTK_CFLAGS@ @SDL_CFLAGS@ 
LIBS = @XMMS_LIBS@ @SDL_LIBS@ @GL_LIBS@ -L../projectM -lprojectM -lGL -lGLU
libprojectm_xmms_la_LDFLAGS = -module -avoid-version
libprojectm_xmms_la_SOURCES =  main.c video_init.h video_init.c


projectm_xmmsconfigdir = /etc/projectM
projectm_xmmsconfig_DATA = config config.fastcomputers config.slowcomputers

projectm_xmmsfontdir = /etc/projectM/fonts
projectm_xmmsfont_DATA = ../../fonts/arial1.glf ../../fonts/times_new1.glf ../../fonts/courier1.glf

projectm_xmmspresetdir = /etc/projectM/presets
projectm_xmmspreset_DATA= 


install-data-hook:
	chown root /etc/projectM -R
	chmod 755 /etc/projectM -R
	cp -r ../../presets_projectM/* /etc/projectM/presets
	chown root /etc/projectM/presets -R
	chmod 777 /etc/projectM/presets -R
