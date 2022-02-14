if(ENABLE_TESTING)
    install(DIRECTORY
            tests
            DESTINATION "${PROJECTM_DATADIR_PATH}/presets"
            COMPONENT Tests
            )
endif()
