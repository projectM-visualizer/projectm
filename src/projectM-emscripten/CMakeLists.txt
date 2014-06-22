PROJECT(projectM-emscripten)
if(COMMAND cmake_policy)
       cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

ADD_EXECUTABLE(projectM-emscripten projectM_SDL_emscripten.cpp)

TARGET_LINK_LIBRARIES(projectM-emscripten  projectM)

INCLUDE_DIRECTORIES(${projectM_SOURCE_DIR})


#pkg_search_module(LIBPROJECTM REQUIRED libprojectM)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake)

#INCLUDE(UseStaticLibs)
#INCLUDE(FindSDL2)

#FIND_PACKAGE(SDL2 REQUIRED)
#INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
#TARGET_LINK_LIBRARIES(projectM-emscripten ${SDL2_LIBRARIES})

SET_TARGET_PROPERTIES(projectM-emscripten PROPERTIES LINK_FLAGS "-o projectM.html --emrun")
#SET_TARGET_PROPERTIES(projectM-emscripten PROPERTIES LINK_FLAGS "--emrun")
