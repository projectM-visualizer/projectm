noinst_LTLIBRARIES=libMilkdropPresetFactory.la

libMilkdropPresetFactory_la_SOURCES= \
BuiltinFuncs.cpp Func.cpp BuiltinParams.cpp IdlePreset.cpp Parser.cpp \
InitCond.cpp PerFrameEqn.cpp CustomShape.cpp \
PerPixelEqn.cpp CustomWave.cpp MilkdropPreset.cpp PerPointEqn.cpp \
Eval.cpp MilkdropPresetFactory.cpp  PresetFrameIO.cpp \
Expr.cpp Param.cpp \
BuiltinFuncs.hpp          Func.hpp                  ParamUtils.hpp\
BuiltinParams.hpp         IdlePreset.hpp            Parser.hpp\
CValue.hpp                InitCond.hpp              PerFrameEqn.hpp\
CustomShape.hpp           InitCondUtils.hpp         PerPixelEqn.hpp\
CustomWave.hpp            MilkdropPreset.hpp        PerPointEqn.hpp\
Eval.hpp                  MilkdropPresetFactory.hpp PresetFrameIO.hpp\
Expr.hpp                  Param.hpp

:
libMilkdropPresetFactory_la_CPPFLAGS = ${my_CFLAGS} \
-I$(top_srcdir)/src/libprojectM
