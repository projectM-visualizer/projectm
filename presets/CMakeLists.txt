if(ENABLE_PRESETS)

    install(DIRECTORY
            presets-cream_of_the_crop
            presets-En_D
            textures
            DESTINATION "${PROJECTM_DATADIR_PATH}/presets"
            COMPONENT Presets
            )

    if(ENABLE_TESTING)
        install(DIRECTORY
                tests
                DESTINATION "${PROJECTM_DATADIR_PATH}/presets"
                COMPONENT Tests
                )
    endif()
endif()
