project(MilkdropPresetFactory)

SET(MilkdropPresetFactory_SOURCES
	BuiltinFuncs.cpp
	Func.cpp MilkdropPreset.cpp
	Param.hpp PresetFrameIO.cpp
	CustomShape.cpp
	Eval.cpp
	MilkdropPresetFactory.cpp
	PerPixelEqn.cpp BuiltinParams.cpp
	InitCond.cpp Parser.cpp
	CustomWave.cpp Expr.cpp
	PerPointEqn.cpp Param.cpp
	PerFrameEqn.cpp
	IdlePreset.cpp
)

IF(NOT MSVC)
	SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF(NOT MSVC)

INCLUDE_DIRECTORIES(${projectM_SOURCE_DIR} ${Renderer_SOURCE_DIR})
LINK_DIRECTORIES(${projectM_BINARY_DIR} ${Renderer_BINARY_DIR})

ADD_LIBRARY(MilkdropPresetFactory STATIC ${MilkdropPresetFactory_SOURCES})
TARGET_LINK_LIBRARIES(MilkdropPresetFactory Renderer)
