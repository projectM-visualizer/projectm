PROJECT(projectM-sdl)

# no fucking idea wtf dont care
cmake_minimum_required(VERSION 2.4.0)
cmake_policy(SET CMP0005 OLD)
if(COMMAND cmake_policy)
       cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
INCLUDE(../cmake/CPack-projectM.cmake)

SET(PROJECTM_INCLUDE ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM/Renderer)

# we're producing an executable called projectMSDL
ADD_EXECUTABLE(projectMSDL projectM_SDL_main.cpp 
  ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM/PresetFactory.cpp
  ${PROJECTM_ROOT_SOURCE_DIR}/libprojectM/KeyHandler.cpp
)

# openGL
FIND_PACKAGE(OpenGL)

# link in projectM
# i have no idea if this is correct
TARGET_LINK_LIBRARIES(projectMSDL projectM ${OPENGL_LIBRARIES})
SET(PROJECTM_LINK ${LIBPROJECTM_LDFLAGS})
INCLUDE_DIRECTORIES(${PROJECTM_INCLUDE})
LINK_DIRECTORIES(${PROJECTM_LINK})

