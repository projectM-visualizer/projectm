SUFFIXES = .hpp .cpp _moc.cpp _qrc.qrc
QT_CXXFLAGS = @QT_CXXFLAGS@
QT_LIBS     = @QT_LIBS@
MOC         = @QT_MOC@
UIC         = @QT_UIC@

projectM_pulseaudio_qtheaders = \
	QPulseAudioDeviceChooser.hpp \
	QPulseAudioDeviceModel.hpp \
	QPulseAudioThread.hpp

nodist_projectM_pulseaudio_moc_sources = $(projectM_pulseaudio_qtheaders:.hpp=_moc.cpp)

.hpp_moc.cpp:
	$(MOC) -o$@ $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $<

ui_PulseDeviceChooserDialog.h: PulseDeviceChooserDialog.ui
	$(UIC) -o $@ $<

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DPROJECTM_PREFIX=\""${prefix}"\" \
	-I${top_srcdir}/src/libprojectM \
	-I${top_srcdir}/src/libprojectM/Renderer \
	-I${top_srcdir}/src/projectM-qt \
	${QT_CXXFLAGS} \
	${libpulse_CFLAGS} \
	-fPIC

AM_CFLAGS = ${my_CFLAGS} \
	-fvisibility=hidden \
	-ffunction-sections \
	-fdata-sections

bin_PROGRAMS = projectM-pulseaudio

projectM_pulseaudio_SOURCES = \
	qprojectM-pulseaudio.cpp \
	QPulseAudioDeviceChooser.cpp \
	QPulseAudioDeviceModel.cpp \
	QPulseAudioThread.cpp \
	PulseDeviceChooserDialog.ui \
	$(projectM_pulseaudio_qtheaders)

nodist_projectM_pulseaudio_SOURCES = \
	$(nodist_projectM_pulseaudio_moc_sources)

projectM_pulseaudio_LDADD = \
	${libpulse_LIBS} \
	../projectM-qt/libprojectM_qt.a \
	../libprojectM/libprojectM.la \
	${QT_LIBS}

projectM_pulseaudio_LDFLAGS = -static ${QT_LIBS}

projectM_pulseaudio_PROGRAM = projectM-pulseaudio

BUILT_SOURCES =               \
    ui_PulseDeviceChooserDialog.h

CLEANFILES = \
	${BUILT_SOURCES} \
	${nodist_projectM_pulseaudio_moc_sources}

desktopdir = $(datadir)/applications
dist_desktop_DATA = projectM-pulseaudio.desktop

icondir = $(datadir)/icons/hicolor/scalable/apps
icon_svg = ${top_srcdir}/src/projectM-qt/images/icons/prjm16-transparent.svg

install-icons:
	mkdir -p $(DESTDIR)$(icondir); \
	$(INSTALL_DATA) $(icon_svg) $(DESTDIR)$(icondir)/projectM.svg

install-data-local: install-icons

dist_man_MANS = projectM-pulseaudio.1
