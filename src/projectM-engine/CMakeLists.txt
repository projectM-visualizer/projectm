PROJECT(projectM)
ADD_LIBRARY(projectM SHARED projectM.cpp FBO.cpp InitCond.cpp 
 Expr.cpp PCM.cpp Parser.cpp Preset.cpp BeatDetect.cpp PerPixelEqn.cpp Param.cpp CustomWave.cpp CustomShape.cpp 
Func.cpp Eval.cpp  PerFrameEqn.cpp PerPointEqn.cpp fftsg.cpp KeyHandler.cpp
 timer.cpp  wipemalloc.cpp 
BuiltinFuncs.cpp BuiltinParams.cpp Renderer.cpp 
PresetLoader.cpp  PresetChooser.cpp PresetFrameIO.cpp PresetMerge.cpp
ConfigFile.cpp  IdlePreset.cpp TextureManager.cpp MoodBar.cpp image_DXT.c image_helper.c SOIL.c stb_image.c  )

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
        TARGET_LINK_LIBRARIES(projectM GLEW m ftgl freetype)
ENDIF(USE_FTGL)

CONFIGURE_FILE("libprojectM.pc.in" "libprojectM.pc" @ONLY)
CONFIGURE_FILE("config.inp.in" "config.inp" @ONLY)

FILE(GLOB presets "presets/*.milk" "presets/*.prjm" "presets/*.tga")
INSTALL(FILES ${presets} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/projectM/presets)
FILE(GLOB fonts "fonts/*.ttf")
INSTALL(FILES ${fonts} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/projectM/fonts)
INSTALL(FILES "${CMAKE_BINARY_DIR}/libprojectM.pc" DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
INSTALL(FILES config.inp DESTINATION ${CMAKE_INSTALL_PREFIX}/share/projectM)
INSTALL(FILES projectM.hpp Preset.hpp event.h dlldefs.h fatal.h PresetFrameIO.hpp PCM.hpp  DESTINATION include/libprojectM)
INSTALL(TARGETS projectM DESTINATION lib)
