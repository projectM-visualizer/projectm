SUFFIXES = .hpp .cpp _moc.cpp _qrc.qrc

projectM_qt_qtheaders = qprojectm_mainwindow.hpp \
	qpresetfiledialog.hpp \
	qplaylistfiledialog.hpp \
	qplaylistmodel.hpp \
	qprojectmconfigdialog.hpp \
	qplaylisttableview.hpp \
	qpresettextedit.hpp \
	qpreseteditordialog.hpp \
	qprojectm.hpp \
	qprojectmwidget.hpp

projectM_qt_moc_sources = $(projectM_qt_qtheaders:.hpp=_moc.cpp)

.hpp_moc.cpp:
	@MOC@ -o$@ $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CPPFLAGS) $(MOC_CPPFLAGS)\
	$(qt_CPPFLAGS) $(qt_LDFLAGS) $(qt_LIBS) $<

ui_qprojectm_mainwindow.h: qprojectm_mainwindow.ui
	@UIC@ -o $@ $<
	
ui_qpreseteditordialog.h: qpreseteditordialog.ui
	@UIC@ -o $@ $<
	
ui_qprojectmconfigdialog.h: qprojectmconfigdialog.ui
	@UIC@ -o $@ $<

application_qrc.cpp: application.qrc
	@RCC@ -o $@ $<


BUILT_SOURCES =               \
    ui_qprojectm_mainwindow.h \
    ui_qpreseteditordialog.h \
    ui_qprojectmconfigdialog.h

noinst_LIBRARIES = libprojectM_qt.a

libprojectM_qt_a_SOURCES = \
	$(projectM_qt_moc_sources) \
	qprojectm_mainwindow.cpp \
	configfile.hpp configfile.cpp \
	qplaylistfiledialog.cpp \
	qplaylistmodel.cpp \
	qprojectmconfigdialog.cpp \
	qpresettextedit.cpp \
	qpreseteditordialog.cpp \
	application_qrc.cpp \
	$(test_moc_sources)


AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-I${top_srcdir}/src/libprojectM \
	-I${top_srcdir}/src/libprojectM/Renderer \
	${QT_CFLAGS} \
	-fPIC

AM_CFLAGS = ${my_CFLAGS} \
	-fvisibility=hidden \
	-ffunction-sections \
	-fdata-sections
	
CLEANFILES = \
	application_qrc.cpp \
	${BUILT_SOURCES} \
	${projectM_qt_moc_sources}
