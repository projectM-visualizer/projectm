set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_executable(projectM-jack-qt
        ConfigFile.h
        ConfigFile.cpp
        qprojectM-jack.cpp
        )

target_compile_definitions(projectM-jack-qt
        PRIVATE
        PROJECTM_PREFIX="${CMAKE_INSTALL_PREFIX}"
        RESOURCE_PREFIX="share/projectM"
        )

target_link_libraries(projectM-jack-qt
        PRIVATE
        projectM-qt
        JACK::JACK
        ${CMAKE_DL_LIBS}
        )

set_target_properties(projectM-jack-qt PROPERTIES
        OUTPUT_NAME projectM-jack
        )

install(TARGETS projectM-jack-qt
        RUNTIME DESTINATION "${PROJECTM_BIN_DIR}"
        COMPONENT Applications
        )
