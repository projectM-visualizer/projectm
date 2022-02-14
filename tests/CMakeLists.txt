if(NOT ENABLE_TESTING)
    return()
endif()

add_executable(projectM-unittest
        ConfigFile.cpp
        ConfigFile.h
        getConfigFilename.cpp
        getConfigFilename.h
        projectM-unittest.cpp
        )

target_compile_definitions(projectM-unittest
        PRIVATE
        PROJECTM_PREFIX="${CMAKE_INSTALL_PREFIX}"
        )

# Test includes a header file from libprojectM with its full path in the source dir.
target_include_directories(projectM-unittest
        PRIVATE
        "${CMAKE_SOURCE_DIR}"
        )

target_link_libraries(projectM-unittest
        PRIVATE
        projectM_static
        SDL2::SDL2
        SDL2::SDL2main
        ${CMAKE_DL_LIBS}
        )

# Normally there's no need to install test applications, but will
# keep it for now to resemble the autotools package structure.
install(TARGETS projectM-unittest
        RUNTIME DESTINATION "${PROJECTM_BIN_DIR}"
        COMPONENT Tests
        )
