PROJECT(projectM)
ADD_LIBRARY(projectM SHARED projectM.cpp projectM.h pbuffer.cpp pbuffer.h InitCond.cpp InitCond.h 
console_interface.cpp Expr.cpp PCM.cpp Parser.cpp Preset.cpp common.h BeatDetect.cpp PCM.h PerPixelEqn.cpp Eval.h 
Param.cpp CustomWave.cpp CustomShape.h CustomShape.cpp Param.h CustomWave.h BeatDetect.h menu.cpp console_interface.h 
Func.h Func.cpp Eval.cpp wipemalloc.h browser.cpp PerFrameEqn.cpp PerPointEqn.cpp editor.cpp fftsg.cpp glConsole.cpp 
CValue.h Expr.h timer.cpp  wipemalloc.cpp PerFrameEqn.h PerPixelEqn.h PerPointEqn.h browser.h BuiltinFuncs.hpp 
BuiltinFuncs.cpp compare.h editor.h event.h fatal.h SplayTree.hpp fftsg.h glConsole.h menu.h timer.h SplayNode.hpp 
BuiltinParams.hpp BuiltinParams.cpp Preset.hpp PresetSwitcher.hpp PresetSwitcher.cpp Renderer.cpp Renderer.hpp 
ParamUtils.hpp PresetLoader.cpp PresetLoader.hpp)

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
INSTALL(FILES projectM.h pbuffer.h PCM.h BeatDetect.h Preset.hpp event.h console_interface.h dlldefs.h fatal.h  PCM.h DESTINATION include/libprojectM)
INSTALL(FILES config DESTINATION share/projectM)
FILE(GLOB presets "presets/*.milk")
INSTALL(FILES ${presets} DESTINATION share/projectM/presets)
FILE(GLOB fonts "fonts/*.ttf")
INSTALL(FILES ${fonts} DESTINATION share/projectM/fonts)
