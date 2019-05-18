if ENABLE_SDL
  PROJECTM_SDL_SUBDIR = projectM-test projectM-sdl
endif

if ENABLE_QT
  PROJECTM_QT_SUBDIR = projectM-qt
endif

if ENABLE_PULSEAUDIO
  PROJECTM_PULSEAUDIO_SUBDIR = projectM-qt projectM-pulseaudio
endif

if ENABLE_JACK
  PROJECTM_JACK_SUBDIR = projectM-qt projectM-jack
endif

if ENABLE_EMSCRIPTEN
  PROJECTM_EMSCRIPTEN_SUBDIR = projectM-emscripten
endif

if ENABLE_GST
  PROJECTM_GST_SUBDIR = projectM-gst
endif

# system headers/libraries/data to install
# for compatibility reasons here as nobase_include
nobase_include_HEADERS = libprojectM/projectM.hpp libprojectM/Common.hpp libprojectM/dlldefs.h libprojectM/event.h libprojectM/fatal.h libprojectM/PCM.hpp

SUBDIRS = libprojectM NativePresets ${PROJECTM_SDL_SUBDIR} ${PROJECTM_QT_SUBDIR} ${PROJECTM_EMSCRIPTEN_SUBDIR} ${PROJECTM_JACK_SUBDIR} ${PROJECTM_PULSEAUDIO_SUBDIR} ${PROJECTM_GST_SUBDIR}
