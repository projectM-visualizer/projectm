PROJECT(projectM)
ADD_LIBRARY(projectM SHARED projectM.cpp projectM.h pbuffer.cpp pbuffer.h InitCond.cpp InitCond.h console_interface.cpp Expr.cpp PCM.cpp Parser.cpp Preset.cpp common.h BeatDetect.cpp PCM.h PerPixelEqn.cpp Eval.h SplayTree.cpp Param.cpp CustomWave.cpp CustomShape.h CustomShape.cpp Param.h CustomWave.h BeatDetect.h Preset.h menu.cpp console_interface.h Func.h Func.cpp Eval.cpp wipemalloc.h browser.cpp PerFrameEqn.cpp PerPointEqn.cpp editor.cpp fftsg.cpp glConsole.cpp CValue.h Expr.h timer.cpp  wipemalloc.cpp PerFrameEqn.h PerPixelEqn.h PerPointEqn.h browser.h builtin_funcs.h compare.h editor.h event.h fatal.h SplayTree.h fftsg.h glConsole.h menu.h timer.h SplayNode.cpp SplayNode.h)

OPTION(USE_FTGL "Use FTGL for on-screen fonts" ON)
OPTION(USE_FBO "Use Framebuffer Objects" ON)
OPTION(USE_DEBUG "Enable Debugging Output" OFF)

ADD_DEFINITIONS(-DLINUX -DUSE_FBO)

FIND_PACKAGE(OpenGL)
 
INCLUDE(FindPkgConfig.cmake)

IF(USE_DEBUG)
	ADD_DEFINITIONS(-DDEBUG)
ENDIF(USE_DEBUG)

TARGET_LINK_LIBRARIES(projectM GLEW m)

IF(USE_FTGL)
	pkg_search_module (FTGL REQUIRED ftgl)
	ADD_DEFINITIONS(-DUSE_FTGL)
	INCLUDE_DIRECTORIES(${FTGL_INCLUDE_DIRS})
	LINK_DIRECTORIES(${FTGL_LDFLAGS})
        TARGET_LINK_LIBRARIES(projectM GLEW m ftgl)
ENDIF(USE_FTGL)

CONFIGURE_FILE("libprojectM.pc.in" "libprojectM.pc" @ONLY)
CONFIGURE_FILE("config.1.00.in" "config.1.00" @ONLY)

INSTALL(FILES "${CMAKE_BINARY_DIR}/libprojectM.pc" DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)

FILE(GLOB presets "presets/*.milk")
INSTALL(FILES ${presets} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/projectM/presets)
FILE(GLOB fonts "fonts/*.ttf")
INSTALL(FILES ${fonts} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/projectM/fonts)
INSTALL(FILES config.1.00 DESTINATION ${CMAKE_INSTALL_PREFIX}/share/projectM)
INSTALL(FILES projectM.h pbuffer.h PCM.h BeatDetect.h Preset.h event.h console_interface.h dlldefs.h fatal.h  PCM.h DESTINATION include/libprojectM)
INSTALL(TARGETS projectM DESTINATION lib)