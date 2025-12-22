set(SPRITE_SHADER_FILES
        Shaders/MilkdropSpriteFragmentGlsl330.frag
        Shaders/MilkdropSpriteVertexGlsl330.vert
        )

include(GenerateShaderResources)
generate_shader_resources(${CMAKE_CURRENT_BINARY_DIR}/SpriteShaders.hpp
        ${SPRITE_SHADER_FILES}
        )

add_library(UserSprites OBJECT
        ${CMAKE_CURRENT_BINARY_DIR}/SpriteShaders.hpp
        Factory.cpp
        Factory.hpp
        MilkdropSprite.cpp
        MilkdropSprite.hpp
        Sprite.hpp
        SpriteException.hpp
        SpriteManager.cpp
        SpriteManager.hpp
        )

target_include_directories(UserSprites
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}"
        )

target_link_libraries(UserSprites
        PRIVATE
        projectM::Eval
        libprojectM::API # For export header
        PUBLIC
        MilkdropPreset
        GLM::GLM
        )
