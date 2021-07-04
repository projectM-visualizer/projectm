add_executable(projectM-jack-sdl
        ConfigFile.h
        ConfigFile.cpp
        projectM-jack.cpp
        video_init.cpp
        video_init.h
        )

target_compile_definitions(projectM-jack-sdl
        PRIVATE
        PROJECTM_PREFIX="${CMAKE_INSTALL_PREFIX}"
        )

target_link_libraries(projectM-jack-sdl
        PRIVATE
        projectM_static
        JACK::JACK
        SDL2::SDL2
        ${CMAKE_DL_LIBS}
        )

install(TARGETS projectM-jack-sdl
        RUNTIME DESTINATION "${PROJECTM_BIN_DIR}"
        COMPONENT Applications
        )
