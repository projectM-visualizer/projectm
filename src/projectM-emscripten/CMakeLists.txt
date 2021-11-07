if(NOT ENABLE_EMSCRIPTEN)
    return()
endif()

# Setting the suffix to .html also creates an HTML file in addition to the .js and .wasm outputs.
set(CMAKE_EXECUTABLE_SUFFIX ".html")

add_executable(projectM_SDL_emscripten
        projectM_SDL_emscripten.cpp
        )

target_link_libraries(projectM_SDL_emscripten
        PRIVATE
        projectM::libprojectM
        )
