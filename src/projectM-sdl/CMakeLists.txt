if(NOT ENABLE_SDL OR NOT ENABLE_SDL_UI)
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

if(MSVC)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.16")
        set_target_properties(projectMSDL
                PROPERTIES
                VS_DPI_AWARE "PerMonitor"
                )
    else()
        message(AUTHOR_WARNING
                "You're using a CMake version less than 3.16 with Visual Studio.\n"
                "The resulting projectMSDL executable will not be DPI-aware and possibly render at a "
                "lower-than-expected resolution on high-DPI displays.")
    endif()
endif()

install(TARGETS projectMSDL
        RUNTIME DESTINATION "${PROJECTM_BIN_DIR}"
        COMPONENT Applications
        )
