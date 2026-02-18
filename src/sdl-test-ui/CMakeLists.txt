if(NOT ENABLE_SDL_UI OR ENABLE_EMSCRIPTEN OR CMAKE_SYSTEM_NAME STREQUAL Android)
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
        glad
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

    add_custom_command(TARGET projectM-Test-UI POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:libprojectM::projectM>
            $<TARGET_FILE:libprojectM::playlist>
            $<TARGET_FILE_DIR:projectM-Test-UI>
            )
endif()
