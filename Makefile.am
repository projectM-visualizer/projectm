ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS=-DDATADIR_PATH='"${pkgdatadir}"'
SUBDIRS=src
PRESETSDIR = presets
EXTRA_DIST=README.md AUTHORS.txt presets fonts $(PRESETSDIR)
CLEANFILES=

# stick apps in bin
# bin_PROGRAMS = $(top_builddir)/bin

# aka /usr/local/share/projectM
pm_data_dir = $(pkgdatadir)
pm_font_dir = $(pm_data_dir)/fonts
pm_presets_dir = $(pm_data_dir)/presets

# files to install
pm_data__DATA = src/libprojectM/config.inp
pm_font__DATA = fonts/Vera.ttf fonts/VeraMono.ttf

# find and install all preset files
install-data-local:
	test -z $(DESTDIR)$(pkgdatadir) || $(MKDIR_P) $(DESTDIR)$(pm_presets_dir)
	find "$(PRESETSDIR)" -type f -exec $(INSTALL_DATA) {} $(DESTDIR)$(pm_presets_dir) \;

# from https://stackoverflow.com/questions/30897170/ac-subst-does-not-expand-variable answer: https://stackoverflow.com/a/30960268
# ptomato https://stackoverflow.com/users/172999/ptomato

src/libprojectM/config.inp: src/libprojectM/config.inp.in Makefile
	$(AM_V_GEN)rm -f $@ $@.tmp && \
	$(SED) -e "s,%datadir%,$(datadir),"g  $< >$@.tmp && \
	chmod a-w $@.tmp && \
	mv $@.tmp $@

CLEANFILES += src/libprojectM/config.inp
