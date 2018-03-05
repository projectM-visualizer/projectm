SUBDIRS=src
PRESETSDIR = presets
EXTRA_DIST=README.md AUTHORS.txt presets fonts src/libprojectM/Renderer/blur.cg src/libprojectM/Renderer/projectM.cg $(PRESETSDIR)
CLEANFILES=

# aka /usr/local/share/projectM
pm_data_dir = $(pkgdatadir)
pm_font_dir = $(pm_data_dir)/fonts
pm_presets_dir = $(pm_data_dir)/presets
pm_shaders_dir = $(pm_data_dir)/shaders

# files to install
pm_data__DATA = src/libprojectM/config.inp
pm_font__DATA = fonts/Vera.ttf fonts/VeraMono.ttf
pm_shaders__DATA = src/libprojectM/Renderer/blur.cg \
                   src/libprojectM/Renderer/projectM.cg

# find and install all preset files
install-data-local:
	test -z $(pkgdatadir) || $(MKDIR_P) $(pm_presets_dir)
	find "$(PRESETSDIR)" -type f -exec $(INSTALL_DATA) {} $(pm_presets_dir) \;
