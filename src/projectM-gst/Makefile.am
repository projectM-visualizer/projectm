SUBDIRS = element

EXTRA_DIST = autogen.sh

GSTREAMER_EXTRA_DEPS = gstreamer-audio-1.0

bin_PROGRAMS = projectM-gst

# source files
projectM_gst_SOURCES = main.cpp

projectM_gst_CXXFLAGS = \
	-I element \
	-g \
	-I${top_srcdir}/src/libprojectM \
	-I${top_srcdir}/src/libprojectM/Renderer \
	${GST_CFLAGS}

projectM_gst_LDADD =
projectM_gst_LDADD += -lasan
projectM_gst_LDADD += ${GST_LIBS}
# projectM_gst_LDFLAGS = -static
projectM_gst_PROGRAM = projectM-gst

ACLOCAL_AMFLAGS = -I m4

run: all
	GST_PLUGIN_PATH=element ./projectM-gst /home/cyber/test.flac
