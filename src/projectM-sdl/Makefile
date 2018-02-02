BUILD=../../build/

SOURCES= \
	pmSDL.cpp \
	projectM_SDL_main.cpp

CPPFLAGS+=-I. -I../libprojectM
CPPFLAGS_macosx-clang+=-F/Library/Frameworks

LDFLAGS_macosx-clang+=-framework OpenGL -F/Library/Frameworks -framework SDL2
LDFLAGS+=-lGL -ldl -lSDL2
LIBRARIES=../libprojectM  \
 ../libprojectM/NativePresetFactory ../libprojectM/MilkdropPresetFactory ../libprojectM/Renderer

PRODUCTS=projectMSDL.exe

include $(BUILD)rules.mk

$(BUILD)rules.mk:
	cd .. && git submodule update --init --recursive
