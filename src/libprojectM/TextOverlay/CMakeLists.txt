
add_library(TextOverlay OBJECT
        ../gltext.h
        ../Renderer/MenuText.cpp
        ../Renderer/MenuText.h
        )

target_include_directories(TextOverlay
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

target_link_libraries(TextOverlay
        PUBLIC
        GLM::GLM
        )

set_target_properties(TextOverlay PROPERTIES
        FOLDER libprojectM
        )
