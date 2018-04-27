if ENABLE_SDL
  PROJECTM_SDL_SUBDIR = projectM-sdl
endif

if ENABLE_QT
  PROJECTM_QT_SUBDIR = projectM-qt projectM-pulseaudio
endif

SUBDIRS=libprojectM NativePresets ${PROJECTM_SDL_SUBDIR} ${PROJECTM_QT_SUBDIR}

# system headers/libraries/data to install
# for compatibility reasons here as nobase_include
nobase_include_HEADERS = libprojectM/projectM.hpp libprojectM/Common.hpp libprojectM/dlldefs.h libprojectM/event.h libprojectM/fatal.h libprojectM/PCM.hpp
