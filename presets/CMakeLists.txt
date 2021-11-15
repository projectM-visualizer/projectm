if(ENABLE_PRESETS)

    install(DIRECTORY
            presets_bltc201
            presets_eyetune
            presets_milkdrop
            presets_milkdrop_104
            presets_milkdrop_200
            presets_mischa_collection
            presets_projectM
            presets_stock
            presets_tryptonaut
            presets_yin
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
