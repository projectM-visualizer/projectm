BUILD=../../build/

SOURCES=projectM.cpp PCM.cpp Preset.cpp fftsg.cpp KeyHandler.cpp \
 timer.cpp wipemalloc.cpp PresetLoader.cpp  PresetChooser.cpp \
 PipelineMerger.cpp ConfigFile.cpp \
 TimeKeeper.cpp PresetFactory.cpp PresetFactoryManager.cpp

CONFIG=libgl

CPPFLAGS+= -I . -I Renderer
CPPFLAGS_linux=-DLINUX
LDFLAGS+=-lgl
SUBDIRS=Renderer MilkdropPresetFactory NativePresetFactory

PRODUCTS=libprojectM.lib

include $(BUILD)rules.mk

$(BUILD)rules.mk:
	cd .. && git submodule update --init --recursive
