ACLOCAL_AMFLAGS=-I m4
AM_CPPFLAGS=-DDATADIR_PATH='"${pkgdatadir}"'
SUBDIRS=src
PRESETSDIR=presets
EXTRA_DIST=README.md AUTHORS.txt presets fonts vendor $(PRESETSDIR)
CLEANFILES=dist

# stick apps in bin
# bin_PROGRAMS = $(top_builddir)/bin

# aka /usr/local/share/projectM
pm_data_dir = $(pkgdatadir)
pm_font_dir = $(pm_data_dir)/fonts

# files to install
pm_data__DATA = src/libprojectM/config.inp
pm_font__DATA = fonts/Vera.ttf fonts/VeraMono.ttf

# find and install all preset files
install-data-local:
	find "$(PRESETSDIR)" -path "$(PRESETSDIR)/tests" -prune -o -type d -exec $(MKDIR_P) "$(DESTDIR)/$(pm_data_dir)/{}" \;
	find "$(PRESETSDIR)" -path "$(PRESETSDIR)/tests" -prune -o -type f -exec $(INSTALL_DATA) "{}" "$(DESTDIR)/$(pm_data_dir)/{}" \;

# from https://stackoverflow.com/questions/30897170/ac-subst-does-not-expand-variable answer: https://stackoverflow.com/a/30960268
# ptomato https://stackoverflow.com/users/172999/ptomato

src/libprojectM/config.inp: src/libprojectM/config.inp.in Makefile
	$(AM_V_GEN)rm -f $@ $@.tmp && \
	$(SED) -e "s,%datadir%,$(datadir),"g  $< >$@.tmp && \
	chmod a-w $@.tmp && \
	mv $@.tmp $@

macOS/Build/Products/Debug/presets:
	mkdir -p macOS/Build/Products/Debug
	ln -s $(PWD)/presets macOS/Build/Products/Debug/

build-mac: macOS/Build/Products/Debug/presets
	xcrun xcodebuild -scheme "projectM SDL" -configuration Debug -derivedDataPath macOS
	@echo "Products built in macOS/Build/Products/Debug"
	open macOS/Build/Products/Debug

# do a macOS build
dist-mac: dist
	./autogen.sh
	make clean
	./configure --disable-sdlframework
	rm -rf dist
	xcrun xcodebuild -scheme "projectM SDL" -configuration Release archive -archivePath build/ProjectM.xcarchive
	xcrun xcodebuild -scheme "iProjectM" -configuration Release archive -archivePath build/iTunes.xcarchive
	mkdir -p dist
# mv build/ProjectM.xcarchive/Products/Applications/projectM\ SDL.app dist/
	mv src/projectM-iTunes/iTunes\ Visualizer.pkg dist/Music.app-plugin-macOS.pkg
	mv src/projectM-sdl/projectM\ SDL.pkg dist/projectM-macOS.pkg
	mv projectM-*.tar.gz dist/
	@echo "Success!\nBuilt iTunes plugin installer and SDL apps in dist/"

CLEANFILES+=src/libprojectM/config.inp
