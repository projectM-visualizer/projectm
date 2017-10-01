BUILD=../../build/

SOURCES= \
	pmSDL.cpp \
	projectM_SDL_main.cpp

CPPFLAGS+=-I. -I../libprojectM
CPPFLAGS_macosx-clang+=-F/Library/Frameworks

LDFLAGS+=-framework OpenGL -F/Library/Frameworks -framework SDL2
LIBRARIES=../libprojectM ../libprojectM/Renderer ../libprojectM/NativePresetFactory ../libprojectM/MilkdropPresetFactory

PRODUCTS=projectMSDL.exe

@echo "OBJDIR: $(OBJDIR)"

include $(BUILD)rules.mk

$(BUILD)rules.mk:
	cd .. && git submodule update --init --recursive
