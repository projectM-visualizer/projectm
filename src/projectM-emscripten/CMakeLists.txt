PROJECT(projectM-emscripten)

# no fucking idea wtf dont care
cmake_minimum_required(VERSION 2.4.0)
cmake_policy(SET CMP0005 OLD)
if(COMMAND cmake_policy)
       cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
INCLUDE(../cmake/CPack-projectM.cmake)

SET(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM/Renderer)

# we're producing an executable called projectEM, it has one source input
ADD_EXECUTABLE(projectEM projectM_SDL_emscripten.cpp 
  ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM/PresetFactory.cpp
  ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM/KeyHandler.cpp
)

# openGL
SET(OPENGL_gl_LIBRARY "fake_gl_lib.a")
FIND_PACKAGE(OpenGL)

# link in projectM
# i have no idea if this is correct
TARGET_LINK_LIBRARIES(projectEM  projectM ${OPENGL_LIBRARIES})
SET(PROJECTM_LINK ${LIBPROJECTM_LDFLAGS})
INCLUDE_DIRECTORIES(${PROJECTM_INCLUDE})
LINK_DIRECTORIES(${PROJECTM_LINK})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s LEGACY_GL_EMULATION=1 ")

SET_TARGET_PROPERTIES(projectEM PROPERTIES LINK_FLAGS " -s REGAL_SYS_EMSCRIPTEN=1 --embed-file build/presets -o build/projectEM.html --emrun")
#SET_TARGET_PROPERTIES(projectEM PROPERTIES LINK_FLAGS "--emrun")
