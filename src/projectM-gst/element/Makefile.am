gstplugin_LTLIBRARIES = libgstprojectmviz.la

# sources used to compile this plug-in
libgstprojectmviz_la_SOURCES = gstprojectmviz.cpp gstprojectmviz.hpp

AM_CPPFLAGS = \
${my_CFLAGS} \
-include $(top_builddir)/config.h \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-I${top_srcdir}/src/libprojectM \
-I${top_srcdir}/src/libprojectM/Renderer \
${GST_CFLAGS}


# compiler and linker flags used to compile this plugin, set in configure.ac
libgstprojectmviz_la_CFLAGS = $(GST_CFLAGS)

libgstprojectmviz_la_LIBADD = $(GST_LIBS) \
	../../libprojectM/libprojectM.la

libgstprojectmviz_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
