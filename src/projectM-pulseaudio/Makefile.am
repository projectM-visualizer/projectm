SUFFIXES = .hpp .cpp _moc.cpp _qrc.qrc

projectM_pulseaudio_qtheaders = \
	QPulseAudioDeviceChooser.hpp \
	QPulseAudioDeviceModel.hpp \
	QPulseAudioThread.hpp

nodist_projectM_pulseaudio_moc_sources = $(projectM_pulseaudio_qtheaders:.hpp=_moc.cpp)

.hpp_moc.cpp:
	@MOC@ -o$@ $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) $(MOC_CPPFLAGS)\
	$(qt_CPPFLAGS) $(qt_LDFLAGS) $(qt_LIBS) $<

ui_PulseDeviceChooserDialog.h: PulseDeviceChooserDialog.ui
	@UIC@ -o $@ $<

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DPROJECTM_PREFIX=\""${prefix}"\" \
	-I${top_srcdir}/src/libprojectM \
	-I${top_srcdir}/src/libprojectM/Renderer \
	-I${top_srcdir}/src/projectM-qt \
	${QT_CFLAGS} \
	${LIBPULSE_CFLAGS} \
	-fPIC

AM_CFLAGS = ${my_CFLAGS} \
	-fvisibility=hidden \
	-ffunction-sections \
	-fdata-sections

bin_PROGRAMS = projectM-pulseaudio

projectM_pulseaudio_SOURCES = \
	$(projectM_pulseaudio_moc_sources) \
	qprojectM-pulseaudio.cpp \
	QPulseAudioDeviceChooser.cpp \
	QPulseAudioDeviceModel.cpp \
	QPulseAudioThread.cpp

projectM_pulseaudio_LDADD = \
	${QT_LIBS} \
	${LIBPULSE_LIBS} \
	${top_srcdir}/src/projectM-qt/libprojectM_qt.a \
	${top_srcdir}/src/libprojectM/libprojectM.la

projectM_pulseaudio_LDFLAGS = -static

projectM_pulseaudio_PROGRAM = projectM-pulseaudio

BUILT_SOURCES =               \
    ui_PulseDeviceChooserDialog.h

CLEANFILES = \
	${BUILT_SOURCES} \
	${projectM_pulseaudio_moc_sources}

desktopdir = $(datadir)/applications
dist_desktop_DATA = projectM-pulseaudio.desktop

dist_man_MANS = projectM-pulseaudio.1
