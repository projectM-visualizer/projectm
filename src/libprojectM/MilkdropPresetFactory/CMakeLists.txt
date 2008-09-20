PROJECT(MilkdropPresetFactory)
cmake_minimum_required(VERSION 2.4.0)

SET(MilkdropPresetFactory_SOURCES BuiltinFuncs.cpp Func.cpp MilkdropPreset.cpp Param.hpp PresetFrameIO.cpp CustomShape.cpp  Eval.cpp MilkdropPresetFactory.cpp PerPixelEqn.cpp BuiltinParams.cpp InitCond.cpp Parser.cpp CustomWave.cpp Expr.cpp PerPointEqn.cpp Param.cpp PerFrameEqn.cpp)

INCLUDE_DIRECTORIES("../" "../Renderer")
ADD_LIBRARY(MilkdropPresetFactory STATIC ${MilkdropPresetFactory_SOURCES})
SET_TARGET_PROPERTIES(MilkdropPresetFactory PROPERTIES VERSION 2.00 SOVERSION 2)
TARGET_LINK_LIBRARIES(MilkdropPresetFactory m)


INSTALL(TARGETS MilkdropPresetFactory DESTINATION lib${LIB_SUFFIX})
