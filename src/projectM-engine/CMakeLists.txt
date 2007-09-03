PROJECT(projectM)
ADD_LIBRARY(projectM SHARED projectM.cpp projectM.hpp PBuffer.cpp PBuffer.hpp InitCond.cpp InitCond.hpp
 Expr.cpp PCM.cpp Parser.cpp Preset.cpp Common.hpp BeatDetect.cpp PCM.hpp PerPixelEqn.cpp Eval.hpp 
Param.cpp CustomWave.cpp CustomShape.hpp CustomShape.cpp Param.hpp CustomWave.hpp BeatDetect.hpp console_interface.h 
Func.hpp Func.cpp Eval.cpp wipemalloc.h PerFrameEqn.cpp PerPointEqn.cpp fftsg.cpp console_interface.cpp
CValue.hpp Expr.hpp timer.cpp  wipemalloc.cpp PerFrameEqn.hpp PerPixelEqn.hpp PerPointEqn.hpp BuiltinFuncs.hpp 
BuiltinFuncs.cpp compare.h event.h fatal.h fftsg.h timer.h BuiltinParams.hpp BuiltinParams.cpp Preset.hpp Renderer.cpp Renderer.hpp ParamUtils.hpp PresetLoader.cpp PresetLoader.hpp PresetChooser.hpp PresetChooser.cpp PresetFrameIO.cpp PresetFrameIO.hpp PresetMerge.cpp PresetMerge.hpp ConfigFile.h ConfigFile.cpp IdlePreset.hpp IdlePreset.cpp bmpLoader.h bmpLoader.c pcxLoader.c pcxLoader.h texture.cpp texture.h tgaLoader.c tgaLoader.h)

OPTION(USE_FTGL "Use FTGL for on-screen fonts" ON)

ADD_DEFINITIONS(-DLINUX -DUSE_FBO)

FIND_PACKAGE(OpenGL)

INCLUDE(FindPkgConfig.cmake)

TARGET_LINK_LIBRARIES(projectM GLEW m)

IF(USE_FTGL)
	pkg_search_module (FTGL ftgl)
	ADD_DEFINITIONS(-DUSE_FTGL)
	INCLUDE_DIRECTORIES(${FTGL_INCLUDE_DIRS})
	LINK_DIRECTORIES(${FTGL_LIBRARY_DIRS} "./")
        TARGET_LINK_LIBRARIES(projectM GLEW m ftgl)
ENDIF(USE_FTGL)

CONFIGURE_FILE("libprojectM.pc.in" "libprojectM.pc" @ONLY)
CONFIGURE_FILE("config.inp.in" "config.inp" @ONLY)

FILE(GLOB presets "presets/*.milk")
INSTALL(FILES ${presets} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/projectM/presets)
FILE(GLOB fonts "fonts/*.ttf")
INSTALL(FILES ${fonts} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/projectM/fonts)
INSTALL(FILES "${CMAKE_BINARY_DIR}/libprojectM.pc" DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
INSTALL(FILES config.inp DESTINATION ${CMAKE_INSTALL_PREFIX}/share/projectM)
INSTALL(FILES projectM.hpp PBuffer.hpp PCM.hpp BeatDetect.hpp Preset.hpp event.h console_interface.h dlldefs.h fatal.h   PresetFrameIO.hpp PCM.hpp Renderer.hpp PresetChooser.hpp DESTINATION include/libprojectM)
INSTALL(TARGETS projectM DESTINATION lib)
