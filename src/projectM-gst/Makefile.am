SUBDIRS = element

EXTRA_DIST = autogen.sh

GSTREAMER_EXTRA_DEPS = gstreamer-audio-1.0

bin_PROGRAMS = projectM-gst

# source files
projectM_gst_SOURCES = main.c

projectM_gst_CFLAGS = \
	-I element \
	-g \
	${GST_CFLAGS}

projectM_gst_LDADD =
projectM_gst_LDADD += -Lelement -lgstprojectmviz
projectM_gst_LDADD += -lasan
projectM_gst_LDADD += ${GST_LIBS}
# projectM_gst_LDFLAGS = -static
projectM_gst_PROGRAM = projectM-gst


ACLOCAL_AMFLAGS = -I m4
