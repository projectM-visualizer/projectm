if(NOT ENABLE_SDL)
    return()
endif()

add_executable(projectMSDL
        audioCapture.cpp
        audioCapture.hpp
        loopback.cpp
        loopback.hpp
        pmSDL.hpp
        pmSDL.cpp
        projectM_SDL_main.cpp
        setup.cpp
        setup.hpp
        )

target_compile_definitions(projectMSDL
        PRIVATE
        PROJECTM_PREFIX="${CMAKE_INSTALL_PREFIX}"
        )

target_link_libraries(projectMSDL
        PRIVATE
        projectM::libprojectM
        GLM::GLM
        SDL2::SDL2
        SDL2::SDL2main
        ${CMAKE_DL_LIBS}
        )

install(TARGETS projectMSDL
        RUNTIME DESTINATION "${PROJECTM_BIN_DIR}"
        COMPONENT Applications
        )
