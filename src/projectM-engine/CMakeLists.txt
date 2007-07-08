PROJECT(projectM)
ADD_LIBRARY(projectM SHARED projectM.cpp projectM.hpp PBuffer.cpp PBuffer.hpp InitCond.cpp InitCond.hpp
 Expr.cpp PCM.cpp Parser.cpp Preset.cpp Common.hpp BeatDetect.cpp PCM.hpp PerPixelEqn.cpp Eval.hpp 
Param.cpp CustomWave.cpp CustomShape.hpp CustomShape.cpp Param.hpp CustomWave.hpp BeatDetect.hpp menu.cpp console_interface.h 
Func.hpp Func.cpp Eval.cpp wipemalloc.h browser.cpp PerFrameEqn.cpp PerPointEqn.cpp editor.cpp fftsg.cpp glConsole.cpp  console_interface.cpp
CValue.h Expr.hpp timer.cpp  wipemalloc.cpp PerFrameEqn.h PerPixelEqn.hpp PerPointEqn.hpp browser.h BuiltinFuncs.hpp 
BuiltinFuncs.cpp compare.h editor.h event.h fatal.h SplayTree.hpp fftsg.h glConsole.h menu.h timer.h SplayNode.hpp BuiltinParams.hpp BuiltinParams.cpp Preset.hpp Renderer.cpp Renderer.hpp ParamUtils.hpp PresetLoader.cpp PresetLoader.hpp PresetChooser.hpp PresetChooser.cpp)

OPTION(USE_FTGL "Use FTGL for on-screen fonts" ON)

ADD_DEFINITIONS(-DLINUX -DFBO)

FIND_PACKAGE(OpenGL)

INCLUDE(FindPkgConfig.cmake)

IF(USE_FTGL)
	pkg_search_module (FTGL ftgl)
	ADD_DEFINITIONS(-DUSE_FTGL)
	INCLUDE_DIRECTORIES(${FTGL_INCLUDE_DIRS})
	LINK_DIRECTORIES(${FTGL_LIBRARY_DIRS})
ENDIF(USE_FTGL)

TARGET_LINK_LIBRARIES(projectM GLEW m)

INSTALL(TARGETS projectM DESTINATION lib)
INSTALL(FILES projectM.hpp PBuffer.hpp PCM.hpp BeatDetect.hpp Preset.hpp event.h console_interface.h dlldefs.h fatal.h   PresetFrameIO.hpp PCM.hpp Renderer.hpp PresetChooser.hpp DESTINATION include/libprojectM)
INSTALL(FILES config DESTINATION share/projectM)
FILE(GLOB presets "presets/*.milk")
INSTALL(FILES ${presets} DESTINATION share/projectM/presets)
FILE(GLOB fonts "fonts/*.ttf")
INSTALL(FILES ${fonts} DESTINATION share/projectM/fonts)
