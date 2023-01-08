if(NOT ENABLE_SDL_UI)
    return()
endif()

add_executable(projectM-Test-UI
        ConfigFile.cpp
        ConfigFile.h
        audioCapture.cpp
        audioCapture.hpp
        loopback.cpp
        loopback.hpp
        pmSDL.cpp
        pmSDL.hpp
        projectM_SDL_main.cpp
        setup.cpp
        setup.hpp
        )

target_link_libraries(projectM-Test-UI
        PRIVATE
        libprojectM::playlist
        GLM::GLM
        SDL2::SDL2
        SDL2::SDL2main
        ${CMAKE_DL_LIBS}
        )

if(MSVC)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.16")
        set_target_properties(projectM-Test-UI
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
