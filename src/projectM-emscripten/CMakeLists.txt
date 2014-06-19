INCLUDE_DIRECTORIES(${projectM_SOURCE_DIR})

ADD_EXECUTABLE(projectM-emscripten projectM_SDL_emscripten.cpp)
#pkg_search_module(LIBPROJECTM REQUIRED libprojectM)
