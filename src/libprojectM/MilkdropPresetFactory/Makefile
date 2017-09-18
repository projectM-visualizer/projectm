BUILD=../../../build/

SOURCES= \
	BuiltinFuncs.cpp \
	Func.cpp MilkdropPreset.cpp \
	PresetFrameIO.cpp \
	CustomShape.cpp \
	Eval.cpp \
	MilkdropPresetFactory.cpp \
	PerPixelEqn.cpp BuiltinParams.cpp \
	InitCond.cpp Parser.cpp \
	CustomWave.cpp Expr.cpp \
	PerPointEqn.cpp Param.cpp \
	PerFrameEqn.cpp \
	IdlePreset.cpp \
	../PresetFactory.cpp \


CPPFLAGS+= -I. -I..
PRODUCTS=MilkdropPresetFactory.lib

include $(BUILD)rules.mk

$(BUILD)rules.mk:
	cd .. && git submodule update --init --recursive
