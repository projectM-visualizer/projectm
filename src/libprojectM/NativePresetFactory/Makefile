BUILD=../../../build/

SOURCES=NativePresetFactory.cpp
CPPFLAGS+= -I. -I..
PRODUCTS=NativePresetFactory.lib

include $(BUILD)rules.mk

$(BUILD)rules.mk:
	cd .. && git submodule update --init --recursive
