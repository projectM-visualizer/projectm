PROJECT(projectM)
ADD_LIBRARY(projectM SHARED projectM.cpp projectM.h pbuffer.cpp pbuffer.h InitCond.cpp InitCond.h console_interface.cpp Expr.cpp PCM.cpp Parser.cpp Preset.cpp common.h BeatDetect.cpp PCM.h PerPixelEqn.cpp Eval.h SplayTree.cpp Param.cpp CustomWave.cpp CustomShape.h CustomShape.cpp Param.h CustomWave.h BeatDetect.h Preset.h menu.cpp console_interface.h Func.h Func.cpp Eval.cpp wipemalloc.h browser.cpp builtin_funcs.cpp PerFrameEqn.cpp PerPointEqn.cpp editor.cpp fftsg.cpp glConsole.cpp CValue.h Expr.h timer.cpp  wipemalloc.cpp PerFrameEqn.h PerPixelEqn.h PerPointEqn.h browser.h builtin_funcs.h compare.h editor.h event.h fatal.h SplayTree.h fftsg.h glConsole.h menu.h timer.h SplayNode.cpp SplayNode.h)
ADD_DEFINITIONS(-DLINUX -DDEBUG -DUSE_FTGL -DFBO)
INCLUDE (${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
INCLUDE_DIRECTORIES(/usr/include/freetype2/)
TARGET_LINK_LIBRARIES(projectM GL GLEW ftgl m)

install(TARGETS projectM DESTINATION lib)
install(FILES projectM.h pbuffer.h PCM.h BeatDetect.h Preset.h event.h console_interface.h dlldefs.h fatal.h  PCM.h DESTINATION include/libprojectM)