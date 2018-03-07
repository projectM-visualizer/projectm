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
projectM_qt_moc_sources = $(projectM_qt_qtheaders:.h=_moc.cpp)
.h_moc.cpp:
	@MOC@ -o$@ $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(MOC_CPPFLAGS)\
  $(qt_CPPFLAGS) $(qt_LDFLAGS) $(qt_LIBS) $<

.h_ui.ui:
@UIC@ -o $@ $<

.cpp_qrc.qrc:
@RCC@ -o $@ $<

bin_PROGRAMS = projectM-qt

projectM_qt_SOURCES = $(projectM_qt_moc_sources) \
qprojectm_mainwindow.cpp \
qprojectm_mainwindow.hpp \
configfile.hpp configfile.cpp \
qpresetfiledialog.hpp \
qplaylistfiledialog.cpp \
qplaylistfiledialog.hpp \
qplaylistmodel.cpp \
qplaylistmodel.hpp \
qxmlplaylisthandler.hpp \
qprojectmconfigdialog.cpp \
qprojectmconfigdialog.hpp \
qplaylisttableview.hpp \
qpresettextedit.cpp \
qpresettextedit.hpp \
qpreseteditordialog.cpp \
qpreseteditordialog.hpp \
qprojectm.hpp \
qprojectmwidget.hpp


AM_CPPFLAGS = \
-include $(top_builddir)/config.h \
-DSYSCONFDIR=\""$(sysconfdir)"\" \
-I${top_srcdir}/src/libprojectM \
-I${top_srcdir}/src/libprojectM/Renderer \
${SDL_CFLAGS}

AM_CFLAGS = ${my_CFLAGS} \
-fvisibility=hidden \
-ffunction-sections \
-fdata-sections
