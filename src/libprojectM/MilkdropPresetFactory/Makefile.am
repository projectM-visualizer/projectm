lib_LTLIBRARIES=libMilkdropPresetFactory.la

libMilkdropPresetFactory_la_SOURCES= \
BuiltinFuncs.cpp Func.cpp BuiltinParams.cpp IdlePreset.cpp Parser.cpp \
InitCond.cpp PerFrameEqn.cpp CustomShape.cpp \
PerPixelEqn.cpp CustomWave.cpp MilkdropPreset.cpp PerPointEqn.cpp \
Eval.cpp MilkdropPresetFactory.cpp  PresetFrameIO.cpp \
Expr.cpp Param.cpp MilkdropPresetFactory.cpp

libMilkdropPresetFactory_la_CPPFLAGS = ${my_CFLAGS} \
-I$(top_srcdir)/src/libprojectM

libMilkdropPresetFactory_la_LDFLAGS = -static
